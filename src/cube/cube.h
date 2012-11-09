#ifndef JetStream_cube_h
#define JetStream_cube_h

#include <iterator>
#include <vector>
#include <list>
#include <map>
#include "dataplaneoperator.h"  //needed only for Receiver
#include "cube_iterator.h"
#include "subscriber.h"

#include <boost/shared_ptr.hpp>
#include "jetstream_types.pb.h"
#include "js_executor.h"

namespace jetstream {
class DataCube;
class TupleProcessingInfo;
typedef std::string DimensionKey;
}
#include "tuple_batch.h"

namespace jetstream {

/**
*  A class to represent a cube in memory.
*/


class TupleProcessingInfo {
  public:
    TupleProcessingInfo(boost::shared_ptr<Tuple> t, DimensionKey key): t(t), key(key), need_new_value(false), need_old_value(false) {};
    boost::shared_ptr<Tuple> t;
    DimensionKey key;
    std::list<operator_id_t> insert;
    std::list<operator_id_t> update;
    bool need_new_value;
    bool need_old_value;
};

class DataCube : public TupleReceiver {
  public:
    typedef jetstream::DimensionKey DimensionKey;

    DataCube(jetstream::CubeSchema _schema, std::string _name, size_t batch=1, size_t num_threads=2, boost::posix_time::time_duration batch_timeout = boost::posix_time::millisec(100));
    virtual ~DataCube() {}

    //main external functions
    virtual void create() = 0;
    virtual void destroy() = 0;
    void process(boost::shared_ptr<Tuple> t);
    size_t batch_size();
    void set_batch_timeout(boost::posix_time::time_duration batch_timeout);


    //query functions
    static unsigned int const LEAF_LEVEL;
    virtual boost::shared_ptr<jetstream::Tuple>
    get_cell_value(jetstream::Tuple const &t, bool final = true) const= 0;

    virtual cube::CubeIterator
    slice_query(jetstream::Tuple const &min, jetstream::Tuple const& max,
                bool final = true, std::list<std::string> const &sort = std::list<std::string>(),
                size_t limit = 0) const = 0;

    virtual cube::CubeIterator rollup_slice_query(std::list<unsigned int> const &levels,
        jetstream::Tuple const &min, jetstream::Tuple const &max, bool final = true,
        std::list<std::string> const &sort = std::list<std::string>(), size_t limit = 0) const = 0;

    virtual void
    do_rollup(std::list<unsigned int> const &levels,jetstream::Tuple const &min, jetstream::Tuple const& max) = 0;


    // info/utility functions
    virtual jetstream::cube::CubeIterator end() const = 0;
    virtual size_t num_leaf_cells() const = 0;
    Tuple empty_tuple();
    const jetstream::CubeSchema& get_schema() ;
    virtual std::string id_as_str() ;
    virtual const std::string& typename_as_str() ;
    virtual void no_more_tuples() {};


    /**
    * It's possible to mark a cube as locked. The intended use of this is to allow
    * graceful deletion. The deleter marks the cube as frozen. As updates to the cube fail,
    * data sources drop their pointer. When the last smart pointer is removed,
    * the cube is deleted.
    *
    * Possibly a different mechanism is needed to do visibility control.
    *
    */
    void mark_as_deleted() {
      is_frozen = true;
    }

    //subscriber stuff
    void add_subscriber(boost::shared_ptr<cube::Subscriber> sub);
    bool remove_subscriber(boost::shared_ptr<cube::Subscriber> sub);
    bool remove_subscriber(operator_id_t id);

    //only used by tuple batch
    virtual DimensionKey get_dimension_key(Tuple const &t) const = 0;

    void save_callback(jetstream::TupleProcessingInfo &tpi,
                       boost::shared_ptr<jetstream::Tuple> new_tuple, boost::shared_ptr<jetstream::Tuple> old_tuple);

    virtual void save_tuple(jetstream::Tuple const &t, bool need_new_value, bool need_old_value,
                            boost::shared_ptr<jetstream::Tuple> &new_tuple,boost::shared_ptr<jetstream::Tuple> &old_tuple)=0;

    virtual void save_tuple_batch(std::vector<boost::shared_ptr<jetstream::Tuple> > tuple_store,
                                  std::vector<bool> need_new_value_store, std::vector<bool> need_old_value_store,
                                  std::list<boost::shared_ptr<jetstream::Tuple> > &new_tuple_list,
                                  std::list<boost::shared_ptr<jetstream::Tuple> > &old_tuple_list)=0;


  virtual boost::shared_ptr<CongestionMonitor> congestion_monitor();

  size_t queued_batches() {return outstanding_batches;}

  protected:
    jetstream::CubeSchema schema;
    std::string name;
    bool is_frozen;


    std::map<operator_id_t, boost::shared_ptr<jetstream::cube::Subscriber> > subscribers;
    boost::shared_ptr<cube::TupleBatch> tupleBatcher;
    virtual void merge_tuple_into(jetstream::Tuple &into, jetstream::Tuple const &update) const=0;
    boost::shared_ptr<cube::TupleBatch> & get_tuple_batcher();
    boost::posix_time::time_duration batch_timeout;


//TODO should figure out how to implement this

  private:
    static const std::string my_tyepename;


    virtual void do_process(boost::shared_ptr<Tuple> t);
    void queue_flush();
    void do_flush(boost::shared_ptr<cube::TupleBatch> tb);
    void post_flush();
    void start_batch_timeout();
    void batch_timer_fired(boost::shared_ptr<cube::TupleBatch> batcher);

    size_t elements_in_batch;
    Executor exec;
    shared_ptr<boost::asio::strand> flushStrand;
    shared_ptr<boost::asio::strand> processStrand;
    int outstanding_batches; //protected by processStrand;
    boost::asio::deadline_timer batch_timeout_timer;
    boost::shared_ptr<CongestionMonitor> congestMon;
};

}

#endif
