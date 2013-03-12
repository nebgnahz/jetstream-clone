#ifndef SUBSCRIBER_2FAEJ0UJ
#define SUBSCRIBER_2FAEJ0UJ

#include "dataplaneoperator.h"
#include "js_counting_executor.h"
#include "jetstream_types.pb.h"

namespace jetstream {

class DataCube;

namespace cube {

class Subscriber: public jetstream::DataPlaneOperator {
  friend class jetstream::DataCube;

//  friend DataCube::add_subscriber(boost::shared_ptr<cube::Subscriber> sub);
  // Would ideally just have the one method as friend but it would be a circular dependency

  protected:
    DataCube * cube;
  
  public:
    enum Action {NO_SEND, SEND, SEND_NO_BATCH, SEND_UPDATE} ;

    Subscriber (): DataPlaneOperator(), cube(NULL), exec(1) {};
    virtual ~Subscriber() {};

    //TODO
    bool has_cube() {
      return cube != NULL;
    };

    virtual void process (boost::shared_ptr<jetstream::Tuple> t);
    virtual Action action_on_tuple(boost::shared_ptr<const jetstream::Tuple> const update) = 0;

    //TODO: should this be inline?
      //These are the thing that should be invoked externally; they call into synchronized code underneath.
    virtual void insert_callback(boost::shared_ptr<jetstream::Tuple> const &update,
                                 boost::shared_ptr<jetstream::Tuple> const &new_value);

    virtual void update_callback(boost::shared_ptr<jetstream::Tuple> const &update,
                                 boost::shared_ptr<jetstream::Tuple> const &new_value, 
                                 boost::shared_ptr<jetstream::Tuple> const &old_value);

      //these are invoked in a synchronized way.
    virtual void post_insert(boost::shared_ptr<jetstream::Tuple> const &update,
                                 boost::shared_ptr<jetstream::Tuple> const &new_value) = 0;

    virtual void post_update(boost::shared_ptr<jetstream::Tuple> const &update,
                                 boost::shared_ptr<jetstream::Tuple> const &new_value, 
                                 boost::shared_ptr<jetstream::Tuple> const &old_value) = 0;

    size_t queue_length();
  
    virtual void no_more_tuples ();

  
  private:
    void set_cube(DataCube  *c ) {cube = c;}
    CountingExecutor exec;

  
    const static std::string my_type_name;
  public:
    virtual const std::string& typename_as_str() {
      return my_type_name;
    }
};

} /* cube */



} /* jetsream */

#endif /* end of include guard: SUBSCRIBER_2FAEJ0UJ */
