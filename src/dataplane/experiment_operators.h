//
//  experiment_operators.h
//  JetStream
//
//  Created by Ariel Rabkin on 10/9/12.
//  Copyright (c) 2012 Ariel Rabkin. All rights reserved.
//

#ifndef JetStream_experiment_operators_h
#define JetStream_experiment_operators_h

#include "dataplaneoperator.h"
#include <string>
#include <iostream>
#include <boost/thread/thread.hpp>


namespace jetstream {


class DummyReceiver: public DataPlaneOperator {
 public:
  std::vector< boost::shared_ptr<Tuple> > tuples;
  virtual void process(boost::shared_ptr<Tuple> t) {
    tuples.push_back(t);
  }
  
  virtual std::string long_description() {
      std::ostringstream buf;
      buf << tuples.size() << " stored tuples.";
      return buf.str();
  }
  
  virtual void no_more_tuples() {} //don't exit at end; keep data available
  
  virtual ~DummyReceiver();

GENERIC_CLNAME
};



/***
 * Operator for emitting a specified number of generic tuples.
 */
class SendK: public DataPlaneOperator {
 public:
  virtual operator_err_t configure(std::map<std::string,std::string> &config);
  virtual void start();
  virtual void stop();
  virtual void process(boost::shared_ptr<Tuple> t);
  void operator()();  // A thread that will loop while reading the file    

    
 protected:
  u_int k; //name of file to read
  boost::shared_ptr<boost::thread> loopThread;
  volatile bool running;
  volatile bool send_now;
  
GENERIC_CLNAME
};  
  
}



#endif
