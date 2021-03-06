
#include "js_utils.h"
#include <glog/logging.h>

#include "topk_tput.h"
#include "querier.h"
#include "node.h"

using namespace ::std;
//using namespace boost;
//using namespace boost::interprocess::ipcdetail;

namespace jetstream {


void MultiRoundSender::start() {
  st = node->get_new_strand();
  chain->strand = st.get();
}

double get_rank_val(boost::shared_ptr<const jetstream::Tuple> t, size_t col) {
  double v = 0;
  const Element& e = t->e(col);
  if (e.has_d_val())
    v = e.d_val();
  else if (e.has_i_val())
    v = e.i_val();
  else
    LOG(FATAL) << "expected a numeric value for column "<< col << "; got " << fmt(*t);
  return v;
}

cube::Subscriber::Action
MultiRoundSender::action_on_tuple(OperatorChain * c, boost::shared_ptr<const jetstream::Tuple> const update) {

  return NO_SEND;
}




void
MultiRoundSender::post_insert(boost::shared_ptr<jetstream::Tuple> const &update,
                                 boost::shared_ptr<jetstream::Tuple> const &new_value) {
	;
}

//called on backfill
void
MultiRoundSender::post_update(boost::shared_ptr<jetstream::Tuple> const &update,
                                 boost::shared_ptr<jetstream::Tuple> const &new_value,
                                 boost::shared_ptr<jetstream::Tuple> const &old_value) {
  ;
}


void
MultiRoundSender::get_bounds(Tuple & my_min, Tuple & my_max, const Tuple & q, int time_col) {
    my_min.CopyFrom(q);
    my_max.CopyFrom(q);
    for (unsigned i = 0; i < rollup_levels.size(); ++i) {
      if (rollup_levels[i] == 0) {
        my_min.mutable_e(i)->CopyFrom(min.e(i));
        my_max.mutable_e(i)->CopyFrom(max.e(i));
      }
    }
}

void
MultiRoundSender::meta_from_downstream(DataplaneMessage & msg) {

  vector<  boost::shared_ptr<Tuple> > data_buf;
  DataplaneMessage round_end_marker;

  int round  = 0;
  take_greatest = msg.tput_sort_key()[0] == '-';
//  VLOG(1) << "TPUT got meta from downstream";
  if ( msg.type() == DataplaneMessage::TPUT_START) {
    //sources send their top k.  TODO what about ties?
    sort_order.push_back(msg.tput_sort_key());
    min.CopyFrom(msg.has_tput_r1_start() ?  msg.tput_r1_start() : cube->empty_tuple());
    max.CopyFrom( msg.has_tput_r1_end() ?  msg.tput_r1_end(): min);


    LOG(INFO) << id() << " doing query; range is " << fmt(min) << " to " << fmt(max);
    if (rollup_levels.size() == 0) {
      for (int i =0; i < msg.rollup_levels_size(); ++i) {
        rollup_levels.push_back( msg.rollup_levels(i));
      }
    }
    LOG_IF(FATAL, rollup_levels.size() != cube->num_dimensions()) << "Got "
        << rollup_levels.size() << " rollup dimensions for cube with "
        << cube->num_dimensions() << "dims";
    cube::CubeIterator it = cube->slice_and_rollup(rollup_levels, min, max, sort_order, msg.tput_k());
    LOG(INFO) << "round-1 slice query returned " << it.numCells() << " cells";

    while ( it != cube->end()) {
      data_buf.push_back(*it);
      it++;
    }
    round = 1;

  } else  if ( msg.type() == DataplaneMessage::TPUT_ROUND_2) {

    size_t total_col = msg.tput_r2_col();
  // Sources send all items >= T  and not in top k.
    cube::CubeIterator it = cube->slice_and_rollup(rollup_levels, min, max, sort_order);
    int count = 0;
    while ( it != cube->end() && count++ < msg.tput_k() ) {
      it++;
    }
    boost::shared_ptr<Tuple> t = *it;
    if (take_greatest) {
      while ( it != cube->end() && get_rank_val(t, total_col) >= msg.tput_r2_threshold()) {
        data_buf.push_back(t);
        it++;
        t = *it;
      }
    } else {
      while ( it != cube->end() && get_rank_val(t, total_col) <= msg.tput_r2_threshold()) {
        data_buf.push_back(t);
        it++;
        t = *it;
      }
    }
    round = 2;
  } else  if ( msg.type() == DataplaneMessage::TPUT_ROUND_3) {

    int emitted = 0;
    for (int i =0; i < msg.tput_r3_query_size(); ++i) {
      int time_col = msg.has_tput_r3_timecol() ? msg.tput_r3_timecol() :  -1;
          //The below is a yucky hack to make sure we do a rollup of the time dimension
      const Tuple& q = msg.tput_r3_query(i);
      Tuple my_min;
      Tuple my_max;
      get_bounds(my_min, my_max, q, time_col);
//      LOG(INFO) << "R3 querying for " << fmt( my_min ) << " to " << fmt(my_max);

      cube::CubeIterator v = cube->slice_and_rollup(rollup_levels, my_min, my_max);
      
      if(v.numCells() == 1) {
        boost::shared_ptr<Tuple> val = *v;
        if (time_col != -1) {
          val->mutable_e(time_col)->set_t_val(my_min.e(time_col).t_val());
          round_end_marker.set_window_end(my_max.e(time_col).t_val());
          //notice that the timestamp in the responses is the minimum of the window
          // and the end-window is the MAX of the window.
        }
        VLOG(1) << "R3 of " << id() << " emitting " << fmt( *(val));
        data_buf.push_back(val);
        emitted ++;
      } else if (v.numCells() == 0) {
        //this is not an error. Something might be a candidate but have no hits on this node.
        LOG(WARNING) << "no matches when querying for " << fmt( my_min ) << " to " <<
              fmt(my_max);
      } else {
        LOG(WARNING) << "too many matches querying for " << fmt( my_min ) <<
              " to " << fmt(my_max) << " ( got " << v.numCells() << ")";
      }
    }
    
//    LOG(INFO) << "end of tput for " << id() << "; emitted " << emitted << " tuples";
//     << " based on " << msg.tput_r3_query_size() << " query terms";
    round = 3;
  } else {
//    DataPlaneOperator::meta_from_downstream(msg);
    return; //no emit
  }

  round_end_marker.set_type(DataplaneMessage::END_OF_WINDOW);
  round_end_marker.set_tput_round(round);

  chain->process(data_buf, round_end_marker);
  LOG(INFO) << id() << " data sent, chain is " << chain << " round is " << round;

}

operator_err_t
MultiRoundCoordinator::configure(std::map<std::string,std::string> &config) {

  memset(bw_after_round, 0, sizeof(bw_after_round));
  memset(round_end_times, 0, sizeof(round_end_times));
  
  if (config.find("num_results") != config.end()) {
    num_results = boost::lexical_cast<int32_t>(config["num_results"]);
  } else
    return operator_err_t("must specify num_results for multi-round top-k");


  if (config.find("sort_column") != config.end()) {
    sort_column = config["sort_column"];
  } else {
    return operator_err_t("must specify sort_column for multi-round top-k");
  }

  if (config.find("min_window_size") != config.end())
    min_window_size = boost::lexical_cast<unsigned>(config["min_window_size"]);
  else
    min_window_size = 1;

  if (config.find("rollup_levels") != config.end()) {
    get_rollup_level_array(config["rollup_levels"], rollup_levels);
  }

  if (config.find("ts_field") != config.end()) {
    ts_field = boost::lexical_cast<int32_t>(config["ts_field"]);
//    total_fields = boost::lexical_cast<int32_t>(config["total_fields"]);

    if (config.find("start_ts") != config.end())
      start_ts = boost::lexical_cast<time_t>(config["start_ts"]);
    else
      start_ts = 0;
    
    window_offset = 0;
    if (config.find("window_offset") != config.end()) {
      window_offset = boost::lexical_cast<time_t>(config["window_offset"]);
      window_offset /= 1000; //convert to seconds from ms
    }
  }
  else {
    ts_field = -1;
    start_ts = 0;
    window_offset = 0;
  }

  wait_for_start = 0;
  if (config.find("wait_for_start") != config.end()) {
    wait_for_start = boost::lexical_cast<time_t>(config["wait_for_start"]);
  }

  just_once = true;
  phase = NOT_STARTED;
  return NO_ERR;
}


void
MultiRoundCoordinator::start() {
  LOG(INFO) << "MultiRoundCoordinator " << id() <<" starting";

  boost::lock_guard<tput_mutex> lock (mutex);
  
    //  chain = future_preds.begin()->second;
  LOG_IF(FATAL, !chain) << "Can't have a coordinator with no chains";

  if (!destcube) {  //note this uses the chain starting at this
    boost::shared_ptr<ChainMember> dest = chain->member(chain->members() -1);
    destcube = boost::dynamic_pointer_cast<DataCube>(dest);
    if (!destcube) {
      LOG(FATAL) << "must attach MultiRoundCoordinator to cube";
    }
  
    string trimmed_name = sort_column[0] == '-' ? sort_column.substr(1) : sort_column;
    total_col = destcube->aggregate_offset(trimmed_name)[0]; //assume only one column for dimension
  }
  TimerSource::start();
}

void
MultiRoundCoordinator::add_chain(boost::shared_ptr<OperatorChain> ch) {
    //  LOG(INFO) << "adding chain " << ch.get();
  boost::lock_guard<tput_mutex> lock (mutex);
  if (ch->member(0).get() == this)
    chain = ch;
  else
    future_preds[ch.get()] = ch;
}

void
MultiRoundCoordinator::chain_stopping(OperatorChain * c) {
  boost::lock_guard<tput_mutex> lock (mutex);
  future_preds.erase(c);
}


  //invoked only via timer
void
MultiRoundCoordinator::start_phase_1(time_t window_end) {
  boost::lock_guard<tput_mutex> lock (mutex);

  if (! running)
    return;

  phase = ROUND_1;
  responses_this_phase = 0;
  DataplaneMessage start_proto;
  
  
  bw_after_round[0] = node->bytes_in.read() + node->bytes_out.read();
  round_end_times[0] = get_msec();
  
  start_proto.set_type(DataplaneMessage::TPUT_START);
  start_proto.set_tput_k(num_results);
  start_proto.set_tput_sort_key(sort_column);
  if (rollup_levels.size() > 0)
    for (unsigned int i = 0; i < rollup_levels.size(); ++i)
      start_proto.add_rollup_levels(rollup_levels[i]);
  else
    for (unsigned int i = 0; i <  destcube->num_dimensions(); ++i)
      start_proto.add_rollup_levels(DataCube::LEAF_LEVEL);
  
  candidates.clear();
  
  predecessors.clear();
  map<OperatorChain *, boost::shared_ptr<OperatorChain> >::iterator preds;
  for (preds =future_preds.begin(); preds != future_preds.end(); ++preds) {
    predecessors.push_back(preds->second);
    LOG(INFO) << "Adding pred " << preds->second->chain_name();
  }
  
  if ( predecessors.size() == 0) {
    LOG(INFO) << "TPUT stalling waiting for a predecessor";
    timer->expires_from_now(boost::posix_time::seconds(2));
    timer->async_wait(boost::bind(&MultiRoundCoordinator::start_phase_1, this, window_end+2));
    return;
  }
  
  
  LOG(INFO) << "starting TPUT, k = " << num_results << " and col is " << sort_column << " (id " << total_col << "). "
      << predecessors.size() << " predecessors. TS field is " << ts_field;

  //todo should set tput_r1_start and tput_r2_start
  if (ts_field > -1) {
    
    dim_filter_start = destcube->empty_tuple();
    dim_filter_end = destcube->empty_tuple();
    
    dim_filter_start.mutable_e(ts_field)->set_t_val( start_ts );
    dim_filter_end.mutable_e(ts_field)->set_t_val( window_end );
    start_ts = window_end + 1;
    
    start_proto.mutable_tput_r1_start()->CopyFrom(dim_filter_start);
    start_proto.mutable_tput_r1_end()->CopyFrom(dim_filter_end);
    
    start_proto.set_rollup_levels(ts_field, 0); //roll up the time period
  }
  
  for (unsigned int i = 0; i < predecessors.size(); ++i) {
    shared_ptr<OperatorChain> pred = predecessors[i];
//    LOG(INFO) << "upwards metadata to " << pred;
    pred->upwards_metadata(start_proto, this);
//    LOG(INFO) << "upwards metadata to " << pred <<  " complete";
  }
}



void
MultiRoundCoordinator::process (
          OperatorChain * c,
          vector<boost::shared_ptr<Tuple> > & tuples,
          DataplaneMessage & msg) {
  
    //  if (tuples->size == 0 && msg.type )
//  LOG(INFO) << "TPUT coordinator processing in phase "<< phase <<", from chain " << c <<
 //   ". message-type: " << msg.type();
  boost::lock_guard<tput_mutex> lock (mutex);

  if ( (phase == ROUND_1)|| (phase == ROUND_2)) {

    for (unsigned i = 0; i < tuples.size(); ++i) {
      boost::shared_ptr<Tuple> t = tuples[i];
      if(!t)
        continue;
      boost::shared_ptr< vector<unsigned int> > levs = destcube->get_leaf_levels();
      DimensionKey k = destcube->get_dimension_key(*t, *levs);
      double v = get_rank_val(t, total_col);

      std::map<DimensionKey,CandidateItem>::const_iterator found = candidates.find(k);
      if(found != candidates.end()) {
        CandidateItem& c = candidates[k];
        c.val += v;
        c.responses ++;
      } else {
        candidates[k] = CandidateItem(v, 1,  *t);
      }
    }
    tuples.clear();
  } else if (phase == ROUND_3) {//just let responses through
//    LOG(INFO) << "passing through " << fmt(*t) << " in TPUT phase 3";
//    return;
  }
//  else
//    LOG(WARNING) << "ignoring input"

  if (msg.type() == DataplaneMessage::END_OF_WINDOW) {
   // check what phase source was in; increment label and counter.
   // If we're done with phase, proceed!
   
    LOG_IF(FATAL, !msg.has_tput_round()) << " a valid end-of-window to a tput controller must have a phase number";
    int sender_round = msg.tput_round();
    if (sender_round == phase) {
      responses_this_phase ++;
      if (responses_this_phase == predecessors.size()) {
      
        LOG(INFO) << id() << " completed TPUT round " << phase << " with " << candidates.size()<< " candidates";

        bw_after_round[phase] = node->bytes_in.read() + node->bytes_out.read();
        round_end_times[phase] = get_msec();
        
        if (candidates.size() == 0) {          
          phase = ROUND_3; //move directly to round 3 and done
        }
        
        responses_this_phase = 0;
        if ( phase == 1) {
          start_phase_2();
        } else if (phase == 2) {
          start_phase_3();
        } else if (phase == 3) {
          
          print_stats();
          if(just_once)
            phase = DONE;
          else {
            LOG(INFO) << "TPUT complete, re-running";
            phase = NOT_STARTED;
          }
          candidates.clear();
        } else {
          LOG(FATAL) << " TPUT should only be in phases 1-3, was " << phase;
          //done!
        }
      }
      if  (phase == 1 || phase == 2)
        msg.clear_type(); //don't forward end-of-window unless we're done;

      
    }
  }
}

void
MultiRoundCoordinator::print_stats() const {

  LOG(INFO) << "**********************************";
  for (int i = 1; i < 4; ++i) {
    msec_t delay = round_end_times[i] - round_end_times[i-1];
    long long bytes = bw_after_round[i] -bw_after_round[i-1];
    LOG(INFO) << "* Stats for " << id() << " k: " <<  num_results << " BW-used phase "
        << i << " : " << bytes << " bytes " << delay << " msec";
  }
  LOG(INFO) << "* Stats for " << id() << " k: " <<  num_results << " BW-used total: "
      << (bw_after_round[3] -bw_after_round[0]) << " bytes "
      << (round_end_times[3] - round_end_times[0]) << " msec";
  LOG(INFO) << "**********************************";
}


/**
   Returns a lower bound on the value for the kth item, by checking
 the kth item using only the partial data we already have.
 */
double
MultiRoundCoordinator::calculate_tau() {
  double tau = 0;
  
  vector<double> vals;
  vals.reserve(candidates.size());
  std::map<DimensionKey, CandidateItem >::iterator iter;
  for (iter = candidates.begin(); iter != candidates.end(); iter++) {
    vals.push_back(iter->second.val);
  }
  sort (vals.begin(), vals.end());

  if (vals.size() < num_results)
    tau = vals[ vals.size() -1];
  else
    if ( sort_column[0] == '-' ) //reverse sort; greatest to least
      tau = vals[vals.size() - num_results];
    else
      tau = vals[num_results-1];
  return tau;
}


void
MultiRoundCoordinator::start_phase_2() {

//	Controller takes partial sums; declare kth one as phase-1 bottom Tau-1.
//	[Needs no per-source state at controller]

  double tau_1 = calculate_tau();

  T1 = tau_1 / predecessors.size();
  VLOG(1) << "tau at start of phase two is " << tau_1 << ". Threshold is " << T1
    << ". " << candidates.size()<< " candidates";
  LOG(INFO) << "Starting processing for phase 2";

  phase = ROUND_2;
  DataplaneMessage start_phase;
  start_phase.set_type(DataplaneMessage::TPUT_ROUND_2);
  start_phase.set_tput_r2_threshold(T1);
  start_phase.set_tput_r2_col(total_col);
  for (unsigned int i = 0; i < predecessors.size(); ++i) {
    shared_ptr<OperatorChain> pred = predecessors[i];
    pred->upwards_metadata(start_phase, this);
  }
  LOG(INFO) << "Send out metadata for phase 2";
}

void
MultiRoundCoordinator::start_phase_3() {

  phase = ROUND_3;

  double tau = calculate_tau();  //called tau-2 in the Cao paper. We are guaranteed that
                                 //it is lower than the true value associated with the kth item
  DataplaneMessage r3_start;
  r3_start.set_type(DataplaneMessage::TPUT_ROUND_3);
  r3_start.set_tput_r3_timecol(ts_field);

  std::map<DimensionKey, CandidateItem >::iterator iter;
  unsigned int pred_size = predecessors.size();
  for (iter = candidates.begin(); iter != candidates.end(); iter++) {
    double upper_bound = (pred_size - iter->second.responses) * T1 + iter->second.val;
    if (upper_bound >= tau) {
      Tuple * t = r3_start.add_tput_r3_query();
      t->CopyFrom(iter->second.example);
    }
  }
  VLOG(1) << "tau at start of phase three is " << tau<< "; total of " << r3_start.tput_r3_query_size() << " candidates";

  for (unsigned int i = 0; i < pred_size; ++i) {
    shared_ptr<OperatorChain> pred = predecessors[i];
    pred->upwards_metadata(r3_start, this);
  }
}


int
MultiRoundCoordinator::emit_data() {
  boost::lock_guard<tput_mutex> lock (mutex);
  LOG(INFO) << "TPUT coordinator is running. Phase is " << phase;
  time_t now = time(NULL);
  time_t window_end = max( now - window_offset, start_ts + min_window_size);

  if (phase == DONE) {
  
    DataplaneMessage msg;
    
    for (unsigned int i = 0; i < predecessors.size(); ++i) {
      shared_ptr<OperatorChain> pred = predecessors[i];
      pred->stop();
    }
    /*
    map<OperatorChain *, boost::shared_ptr<OperatorChain> >::iterator preds;
    for (preds =future_preds.begin(); preds != future_preds.end(); ++preds) {
      preds->second->stop();
    }*/

      //signal that this chain is really done.
//    msg.set_type(DataplaneMessage::NO_MORE_DATA);
 //   std::vector<shared_ptr<Tuple> > no_tuples;
 //   chain->process(no_tuples, msg);
  
    return -1;
  }
  if (window_end > now)
    return 1000 * (window_end - now);
  else if (wait_for_start > 0) {
    msec_t wait = 1000 * wait_for_start;
    wait_for_start = 0;
      return wait;
  }
  else {
    if (phase == NOT_STARTED) {
      start_phase_1(window_end);
      return min_window_size;
    }
  }
  return 1000;
//  LOG(INFO) << "Timer will expire in " << timer->expires_from_now();
}


const string MultiRoundSender::my_type_name("TPUT Multi-Round sender");
const string MultiRoundCoordinator::my_type_name("TPUT Multi-Round coordinator");


}
