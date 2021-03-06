/*
 * functions related to the hypercube data structure
 *
 */
#include "cube.h"
#include <glog/logging.h>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

//using namespace ::std;
using namespace jetstream;
using namespace boost;

unsigned int const jetstream::DataCube::LEAF_LEVEL =  100000;
  //std::numeric_limits<unsigned int>::max();

const int BASE_QUEUE_SIZE = 600000 /200; //tuples/sec / wait time
ProcessCallable::ProcessCallable(DataCube * cube, std::string name): name(name),
   //service_process(new io_service(1)),
#if NONBLOCK_QUEUE
   process_tasks(BASE_QUEUE_SIZE),
#endif
  service_flush(new io_service(1)),
 //  work_process(*service_process),
   work_flush(*service_flush),
  cube(cube), tupleBatcher(new cube::TupleBatch(cube)) {
  cntr = 0;
  process_is_running = true;
  
  // this should always be the last line in the constructor
  thread_process = boost::thread(&ProcessCallable::run_process, this);
  thread_flush = boost::thread(&ProcessCallable::run_flush, this);
}


ProcessCallable::~ProcessCallable() {
//  service_process->stop();
  service_flush->stop();
  process_is_running = false;
#if BLOCKING_QUEUE
  has_data.notify_all();
#endif
  thread_process.join();
  thread_flush.join();
}

void ProcessCallable::run_process() {
  jetstream::set_thread_name("js-cube-proc-"+name);

#if BLOCKING_QUEUE
  
  while(process_is_running) {
    boost::unique_lock<boost::mutex> lock(process_lock);
    if(process_tasks.empty()) {
      has_data.wait(lock);
    }
    while(!process_tasks.empty()) {
      ProcessThreadTask * task = process_tasks.front();
      process_tasks.pop();

#elif NONBLOCK_QUEUE
  while(process_is_running) {
    ProcessThreadTask * task = 0;

    while(!process_tasks.pop(task) && process_is_running) {
      js_usleep(5 * 1000);
    }
    if (!task)
      break;
    {
#endif
      if (task->is_insert) {
        TupleInsertion * raw = dynamic_cast<TupleInsertion*>(task);
        cube->do_process(raw->chain, raw->tuple, raw->key, raw->levels, tupleBatcher, this);
      } else {
        FlushTask * flush = dynamic_cast<FlushTask*>(task);
        barrier_to_flushqueue(flush->flush);
      }
      delete task;
    }
  }
 //  service_process->run();
}

void ProcessCallable::run_flush() {
  jetstream::set_thread_name("js-cube-flush-"+name);
  service_flush->run();
}


void ProcessCallable::assign(OperatorChain * chain, const boost::shared_ptr<Tuple> & t, DimensionKey key, const  boost::shared_ptr<std::vector<unsigned int> >& levels) {
  TupleInsertion * ins =  new TupleInsertion(chain, t, key, levels);
#if BLOCKING_QUEUE
  process_lock.lock();
  process_tasks.push(ins);
//  if ((cntr++ & 0x7F) == 0)
    has_data.notify_one();
  process_lock.unlock();
#elif NONBLOCK_QUEUE
  process_tasks.push(ins);
#endif
//  service_process->post(boost::bind(&ProcessCallable::process, this, chain, t, key, levels));
}


void
ProcessCallable::barrier(boost::shared_ptr<FlushInfo> flush) {
//  service_process->post(boost::bind(&ProcessCallable::barrier_to_flushqueue, this, flush));
#if BLOCKING_QUEUE
  process_lock.lock();
  process_tasks.push(new FlushTask(flush));
  has_data.notify_one();
  process_lock.unlock();
#elif NONBLOCK_QUEUE
  process_tasks.push(new FlushTask(flush));
#endif
}

void
ProcessCallable::barrier_to_flushqueue(boost::shared_ptr<FlushInfo> flush) {
  service_flush->post(boost::bind(&ProcessCallable::do_barrier, this, flush));
}

void
ProcessCallable::do_barrier(boost::shared_ptr<FlushInfo> flush) {
  unsigned v = flush->dec_and_get();
  if (v == 1) { // note that dec returns PREVIOUS value
    flush->subsc->flush_callback(flush->id);
  }
}
boost::shared_ptr<cube::TupleBatch>
ProcessCallable::batch_flush() {
  boost::lock_guard<boost::mutex> lock(batcherLock);
  boost::shared_ptr<cube::TupleBatch> ptr = tupleBatcher;
  cube::TupleBatch * batch = new cube::TupleBatch(cube);
  tupleBatcher.reset(batch);
  return ptr;
}

bool ProcessCallable::batcher_ready() {
  return !tupleBatcher->is_empty();
}

void ProcessCallable::check_flush() {
  service_flush->post(boost::bind(&ProcessCallable::do_check_flush, this));
}

void ProcessCallable::do_check_flush() {
  while(batcher_ready()) {
    boost::shared_ptr<cube::TupleBatch> tb = batch_flush();
    size_t batch_size = tb->size();
    VLOG_EVERY_N(1, 1000) << "Flushing with size "<< tb->size() << " thread id " << boost::this_thread::get_id()
       << " size: " << batch_size 
       << " Current flushCongestMon = " << cube->flushCongestMon->queue_length()
       << " Current processhCongestMon = " << cube->processCongestMon->queue_length();
     tb->flush();
     cube->flushCongestMon->report_delete(tb.get(), batch_size);
  }
}



DataCube::DataCube(jetstream::CubeSchema _schema, std::string _name, const NodeConfig &conf) :
  schema(_schema), name(_name), is_frozen(false),
  config(conf),in_chain_count(0),version(0),
  flushCongestMon(boost::shared_ptr<QueueCongestionMonitor>(new QueueCongestionMonitor(conf.cube_congestion_flush_limit, "cube " + _name + " flush"))),
  processCongestMon(boost::shared_ptr<ChainedQueueMonitor>(new ChainedQueueMonitor(conf.cube_congestion_process_limit, "cube " + _name + " process")))
{
  processCongestMon->set_next_monitor(flushCongestMon);

//  LOG(INFO) << "Starting cube with "<<conf.cube_processor_threads <<" threads ";
  for(size_t i=0; i<conf.cube_processor_threads;i++) {
    boost::shared_ptr<ProcessCallable> proc(new ProcessCallable(this, boost::lexical_cast<std::string>(i)));
    processors.push_back(proc);
  }

};

const std::string jetstream::DataCube::my_tyepename("Data Cube");

// Borrowed from the boost hash.hpp; stripped out all the genericization.
unsigned fast_string_hash(const char * c) {
  unsigned seed = 0;
  while (*c != '\0') {
    seed ^= *c + 0x9e3779b9 + (seed<<6) + (seed>>2);
    c++;
  }
  return seed;
}

void DataCube::process(OperatorChain * chain, boost::shared_ptr<Tuple> t) {
//  LOG(INFO) << "processing" << fmt(*t);

  if(config.cube_max_stage < 1)
    return;
  static boost::thread_specific_ptr<std::ostringstream> tmpostr;
  static boost::thread_specific_ptr<boost::hash<std::string> > hash_fn;

  if (!tmpostr.get())
    tmpostr.reset(new std::ostringstream());
/*  if(!hash_fn.get()) {
    LOG(INFO) << "Using thread in process thread_id is: " << boost::this_thread::get_id();
    hash_fn.reset(new boost::hash<std::string>());
  }*/
  if(config.cube_max_stage > 4)
    processCongestMon->report_insert(t.get(), 1);
  tmpostr->str("");
  tmpostr->clear();

  boost::shared_ptr<std::vector<unsigned int> > levels;  
  if (chain && current_levels.find(chain) != current_levels.end()) {
    levels = current_levels[chain];
    get_dimension_key(*t, *levels, *tmpostr);
  }
  else {
    levels = get_leaf_levels();
//    LOG(WARNING) << "Processing tuple data from unrecognized chain " << chain;
    get_dimension_key(*t, *levels, *tmpostr);
  }

  DimensionKey key = tmpostr->str();
//  size_t kh = (*hash_fn)(key);
  size_t kh = fast_string_hash(key.c_str());
  if(config.cube_max_stage < 2)
     return;
  processors[kh % processors.size()]->assign(chain, t, key, levels);
}


void
DataCube::process(OperatorChain * chain,  std::vector<boost::shared_ptr<Tuple> > & tuples, DataplaneMessage& msg) {
  for(unsigned i =0; i < tuples.size(); ++i) {
    if (tuples[i])
      process(chain, tuples[i]);
  }

  if (msg.has_type() && msg.type() != DataplaneMessage::DATA)
  {
    LOG_IF(FATAL, !chain) << "can't process meta from a non-chain";
    shared_lock<boost::shared_mutex> lock(subscriberLock);
    for(std::map<operator_id_t, boost::shared_ptr<jetstream::cube::Subscriber> >::iterator it = subscribers.begin();
        it != subscribers.end(); ++it) {
      boost::shared_ptr<jetstream::cube::Subscriber> sub = (*it).second;
      shared_ptr<FlushInfo> f = sub->incoming_meta(*chain, msg);
      if (f) {
        f->subsc = sub;
        flush(f);
      }
    }
  }

  if (msg.type() == DataplaneMessage::ROLLUP_LEVELS) {
    if (!chain || current_levels.find(chain) != current_levels.end()) {
      LOG(WARNING) << "Recieved rollup levelS info for unrecognized chain " << chain << ", ignoring";
    }
    else if(msg.rollup_levels_size() == 0) {
      set_current_levels(chain, get_leaf_levels());
    }
    else {
      LOG_IF(FATAL, (unsigned int) msg.rollup_levels_size() != num_dimensions()) << "got a rollup levels msg with the wrong number of dimensions: "
        << msg.rollup_levels_size()<< " should be " << num_dimensions();
      std::vector<unsigned int> levels;
      for(int i = 0; i < msg.rollup_levels_size(); ++i ) {
        levels.push_back(msg.rollup_levels(i));
      }
      set_current_levels(chain, levels);
          //chains, at different rollup levels.
    }
  }

  if (msg.type() == DataplaneMessage::NO_MORE_DATA) {
    current_levels.erase(chain);
  }
}


void
DataCube::add_chain(boost::shared_ptr<OperatorChain> c) {
  if (c->members() > 0 && c->member(0).get() != this) {
    boost::interprocess::ipcdetail::atomic_inc32(&in_chain_count);
//  unsigned ccount = boost::interprocess::ipcdetail::atomic_read32(&in_chain_count);
//  LOG(INFO) << "Adding chain into " << name << ", leaving " << ccount;

    // Initialize the rollup levels for this incoming chain
    set_current_levels(c.get(), get_leaf_levels());
  }
}


void
DataCube::chain_stopping(OperatorChain * c) {
  if (c->member(0).get() != this)
    boost::interprocess::ipcdetail::atomic_dec32(&in_chain_count);
  VLOG(1) << "Cube " << name << " got chain-stopping; " << in_chain_count << " in-chains left"; // << c->chain_name();
  DataplaneMessage end_of_chain;
  end_of_chain.set_type(DataplaneMessage::NO_MORE_DATA);
  std::vector< boost::shared_ptr<Tuple> > no_tuples;
  process(c, no_tuples, end_of_chain);
}


void
DataCube::flush(boost::shared_ptr<FlushInfo> f) {
  f->set_count(processors.size());
  for (unsigned i = 0; i < processors.size(); ++i)
    processors[i]->barrier(f);
}

void
DataCube::do_process( OperatorChain * chain,
                      boost::shared_ptr<Tuple> t,
                      DimensionKey key,
                      boost::shared_ptr<std::vector<unsigned int> > levels,
                      boost::shared_ptr<cube::TupleBatch> &tupleBatcher,
                      ProcessCallable * proc) {
  if(config.cube_max_stage < 3)
     return;
  boost::lock_guard<boost::mutex> lock(proc->batcherLock);

  bool in_batch = false;

  VLOG(2) << "Processing " << key  << " thread id " << boost::this_thread::get_id();

  if (tupleBatcher->contains(key)) {
    in_batch = true;
  }

  boost::shared_ptr<TupleProcessingInfo> tpi;
  if(!in_batch) {
    tpi = make_shared<TupleProcessingInfo>(t, key, levels);
  }
  else {
    tpi = tupleBatcher->get(key);
    merge_tuple_into(*(tpi->t), *t);
  }

  bool can_batch = true;
  { //lock
  shared_lock<boost::shared_mutex> lock(subscriberLock);
  for(std::map<operator_id_t, boost::shared_ptr<jetstream::cube::Subscriber> >::iterator it = subscribers.begin();
      it != subscribers.end(); ++it) {
    boost::shared_ptr<jetstream::cube::Subscriber> sub = (*it).second;
    cube::Subscriber::Action act = sub->action_on_tuple(chain, t);
    if(!in_batch) {
      tpi->need_new_value = sub->need_new_value(t);
      tpi->need_old_value = sub->need_old_value(t);
    }
    //LOG(INFO) << "Action: "<< act << "send is: " <<  cube::Subscriber::SEND;
    if(act == cube::Subscriber::SEND) {
      if(!in_batch) {
        VLOG(3) << "Action: "<< act << " adding to insert: " <<  tpi->key;
        tpi->insert.push_back((*it).first);
      }
    }
    else if(act == cube::Subscriber::SEND_NO_BATCH) {
      if(!in_batch) {
        VLOG(3) << "Action: "<< act << " adding to insert: " <<  tpi->key;
        tpi->insert.push_back((*it).first);
      }
      can_batch = false;
    }
    else if(act == cube::Subscriber::SEND_UPDATE) {
      if(!in_batch) {
        VLOG(3) << "Action: "<< act << " adding to update: " <<  tpi->key;
        tpi->update.push_back((*it).first);
      }
    }
  }
  } //lock

  VLOG(2) <<"Process: "<< key << "in batch: "<<in_batch << " can batch:" << can_batch << " need new:" << tpi->need_new_value << " need old:"<< tpi->need_old_value;

  //LOG(INFO) << "In do_process. " << elements_in_batch  ;

  bool was_empty=tupleBatcher->is_empty();

  if(!in_batch) {
    tupleBatcher->insert_tuple(tpi, can_batch);
    if(can_batch)
      flushCongestMon->report_insert(tpi.get(), 1);
  }
  else {
//    LOG_IF(tpi->)
    tupleBatcher->update_batched_tuple(tpi, can_batch);
  }

  if(!tupleBatcher->is_empty() && was_empty)
  {
    proc->check_flush();
  }
  
  processCongestMon->report_delete(t.get(), 1);


}

void DataCube::wait_for_commits() {
  while(flushCongestMon->queue_length() > 0 || processCongestMon->queue_length() > 0)
  {
    js_usleep(processCongestMon->queue_length() + (flushCongestMon->queue_length()/10));
  }
}


void DataCube::save_callback(jetstream::TupleProcessingInfo &tpi, boost::shared_ptr<jetstream::Tuple> new_tuple, boost::shared_ptr<jetstream::Tuple> old_tuple) {
    shared_lock<boost::shared_mutex> lock(subscriberLock);

    for( std::list<operator_id_t>::iterator it=tpi.insert.begin(); it != tpi.insert.end(); ++it) {
      VLOG(3) << "Insert Callback:" <<tpi.key<<"; sub:"<<*it;
      if(subscribers.count(*it) > 0)
        subscribers[*it]->insert_callback(tpi.t, new_tuple);
    }

    for( std::list<operator_id_t>::iterator it=tpi.update.begin(); it != tpi.update.end(); ++it) {
      VLOG(3) << "Update Callback:" <<tpi.key<<"; sub:"<<*it;
      if(subscribers.count(*it) > 0)
        subscribers[*it]->update_callback(tpi.t, new_tuple, old_tuple);
    }
  VLOG(2) << "End Save Callback:" <<tpi.key;
}


void DataCube::add_subscriber(boost::shared_ptr<cube::Subscriber> sub) {
  lock_guard<boost::shared_mutex> lock(subscriberLock);

  LOG_IF(FATAL, sub->has_cube()) << "can't attach subscriber" << sub->id() << " to cube " << name<<
    "; it is already attached to " << sub->cube->name;
//  assert(!sub->has_cube()); //for now, assume subscriber-cube matching is permanent
  sub->set_cube(this);
  subscribers[sub->id()] = sub;
  LOG(INFO) << "Adding subscriber "<< sub->id() << " to " << id_as_str();
}

void DataCube::remove_subscriber(boost::shared_ptr<cube::Subscriber> sub) {
  return remove_subscriber(sub->id());
}

void DataCube::remove_subscriber(operator_id_t id) {
  lock_guard<boost::shared_mutex> lock(subscriberLock);
  subscribers.erase(id);
}

Tuple DataCube::empty_tuple() {
  Tuple t;

  for (int i=0; i < schema.dimensions_size(); ++i) {
    t.add_e();
  }

  return t;
}

void DataCube::set_current_levels(OperatorChain * chain, const std::vector<unsigned int> &levels) {
  current_levels[chain] = make_shared<std::vector<unsigned int> >(levels);
}

void DataCube::set_current_levels(OperatorChain * chain, boost::shared_ptr<std::vector<unsigned int> > levels) {
  current_levels[chain] = levels;
}

const jetstream::CubeSchema& DataCube::get_schema() {
  return schema;
}


std::string DataCube::id_as_str() const {
  return name;
}


const std::string& DataCube::typename_as_str() const {
  return my_tyepename;
}

unsigned
DataCube::in_chains() {
  return boost::interprocess::ipcdetail::atomic_read32(&in_chain_count);
}


