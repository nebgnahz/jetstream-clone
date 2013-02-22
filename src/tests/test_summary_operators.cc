
#include <map>
#include <iostream>
#include <gtest/gtest.h>

#include "experiment_operators.h"
#include "summary_operators.h"
#include "quantile_est.h"


using namespace jetstream;
using namespace boost;
using namespace std;


TEST(Operator, QuantileAndCountOperators) {

  SummaryToCount op;
  shared_ptr<QuantileOperator> q_op(new QuantileOperator);
  shared_ptr<DummyReceiver> receive(new DummyReceiver);

  operator_config_t cfg;
  cfg["q"] = "0.6";
  cfg["field"] = "1";

  operator_err_t err = q_op->configure(cfg);
  ASSERT_EQ(NO_ERR, err);
  err = op.configure(cfg);
  ASSERT_EQ(NO_ERR, err);

  op.set_dest(q_op);
  q_op->set_dest(receive);

  LogHistogram lh(500);
  lh.add_item(2, 1);
  lh.add_item(4, 1);
  lh.add_item(6, 1);


  boost::shared_ptr<Tuple> t(new Tuple);
  extend_tuple(*t, 2);
  extend_tuple(*t, lh);

  op.process(t);

  ASSERT_EQ((size_t)1, receive->tuples.size());
  boost::shared_ptr<Tuple> result = receive->tuples[0];
  ASSERT_EQ(2, result->e(0).i_val());  //first element preserved
  ASSERT_EQ(4, result->e(1).i_val()); //median is 4
  ASSERT_EQ(3, result->e(2).i_val()); //three values
}


TEST(Operator, ToSummary) {
  ToSummary op;
  shared_ptr<DummyReceiver> receive(new DummyReceiver);
  operator_config_t cfg;
  //cfg["size"] = "50";
  cfg["field"] = "0";
  operator_err_t err = op.configure(cfg);
  ASSERT_EQ(NO_ERR, err);
  op.set_dest(receive);


  boost::shared_ptr<Tuple> t(new Tuple);
  extend_tuple(*t, 2);

  op.process(t);
  ASSERT_EQ((size_t)1, receive->tuples.size());
  boost::shared_ptr<Tuple> result = receive->tuples[0];

  ASSERT_EQ(2, result->e(0).summary().items(0));
  //LogHistogram hist(result->e(0).summary());
  //ASSERT_EQ( size_t(1), hist.count_in_b(hist.bucket_with(2)) );
  //ASSERT_EQ( size_t(1), hist.pop_seen() );

}

