#ifndef AGGREGATE_AVG_Q8TYGR7Q
#define AGGREGATE_AVG_Q8TYGR7Q

#include "aggregate.h"
#include <glog/logging.h>

namespace jetstream {
namespace cube {

/**
 * @brief Aggregate for avgs
 *
 * input tuple format:
 *
 * single entry: [value to be averaged as int or double]
 * partial aggregate: [sum as int or double] [count of items in sum as int]
 *
 */
class MysqlAggregateAvg: public MysqlAggregate {
  public:
    MysqlAggregateAvg() : MysqlAggregate() {};

    std::vector<std::string> get_column_types() const;

    std::vector<std::string> get_column_names() const;
    std::string get_update_on_insert_sql() const;

    virtual void insert_default_values_for_full_tuple(jetstream::Tuple &t) const;
    virtual size_t number_tuple_elements() const;
    
    virtual void set_value_for_insert_tuple(
      boost::shared_ptr<sql::PreparedStatement> pstmt, jetstream::Tuple const &t,
      int &field_index);
    
    virtual void populate_tuple_final(boost::shared_ptr<jetstream::Tuple> t, boost::shared_ptr<sql::ResultSet> resultset, int &column_index) const ;

    void populate_tuple_partial(boost::shared_ptr<jetstream::Tuple> t, boost::shared_ptr<sql::ResultSet> resultset, int &column_index) const ;
    
    virtual std::string get_select_clause_for_rollup() const;

    virtual void update_from_delta(jetstream::Tuple & newV, const jetstream::Tuple& oldV) const;


  protected:
    virtual void merge_full_tuple_into(jetstream::Tuple &into, jetstream::Tuple const &update) const;
    void merge_sum(jetstream::Element * into, jetstream::Element * const update) const;
    
    virtual void set_value(
      boost::shared_ptr<sql::PreparedStatement> pstmt,int &field_index, jetstream::Element *const sum) const;
    
    virtual void set_value(
      boost::shared_ptr<sql::PreparedStatement> pstmt,int &field_index, jetstream::Element *const sum, jetstream::Element *const count) const;
};

} /* cube */
} /* jetstream */


#endif /* end of include guard: AGGREGATE_AVG_Q8TYGR7Q */
