/**
 *  Tests for various system operators, such as ReadLine
 */

#include "base_operators.h"
#include "chain_ops.h"
#include "variable_sampling.h"
#include <map>
#include <iostream>
#include "node.h"
#include <gtest/gtest.h>

#include "experiment_operators.h"

using namespace jetstream;
//using namespace boost;
using boost::shared_ptr;
//using namespace std;
using std::string;
using std::cout;
using std::endl;
using std::map;
using std::vector;



TEST(Operator, FixedSampleOperator) {

  FixedSampleOperator degrade_op;

  operator_config_t cfg;
  cfg["max_drops"] = "4";
  degrade_op.configure(cfg);

  boost::shared_ptr<CongestionPolicy> policy(new CongestionPolicy);
  boost::shared_ptr<QueueCongestionMonitor> mockCongest(new QueueCongestionMonitor(256, "dummy"));
  mockCongest->set_downstream_congestion(0.5);
  policy->set_congest_monitor(mockCongest);
  policy->add_operator(degrade_op.id());
//  policy->clear_last_action();

  degrade_op.set_congestion_policy(policy);
  degrade_op.start();

  Tuple t;
  extend_tuple(t, 2);

  for(int i = 0; i < 10; ++i) {
    bool should_emit = degrade_op.should_emit(t);
    ASSERT_EQ( should_emit, i%2 != 0);
  }
  
//  cout << fmt(*result) << endl;

}