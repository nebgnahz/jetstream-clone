#ifndef _node_H_
#define _node_H_

#include <sys/types.h>
#include <boost/thread.hpp>

#include "js_utils.h"
#include "jetstream_types.pb.h"
#include "dataplaneoperator.h"
#include "dataplane_operator_loader.h"
#include "connection.h"
#include "cube_manager.h"
#include "liveness_manager.h"
#include "dataplane_comm.h"
#include "node_web_interface.h"
#include "node_config.h"

namespace jetstream {
  
class net_interface;

class Node {
  friend class NodeWebInterface;

 private:
  NodeConfig config;
  boost::shared_ptr<boost::asio::io_service> iosrv;
  boost::shared_ptr<ConnectionManager> connMgr;
  LivenessManager livenessMgr;
  NodeWebInterface  webInterface;
  CubeManager cubeMgr;
  DataplaneConnManager dataConnMgr;
  boost::shared_ptr<ServerConnection> listeningSock;
  std::vector<boost::shared_ptr<ClientConnection> > controllers;
  mutable boost::mutex threadpoolLock; // a mutex to make sure concurrent thread starts/stops
            //don't interfere
  mutable boost::mutex operatorTableLock; // protects list of operators
  
  boost::condition_variable startStopCond;
  OperatorCleanup operator_cleanup;
            
  // I don't think we need this
  //  std::vector<boost::shared_ptr<ClientConnection> > peers;  
  // perhaps we should keep a map from dest to socket instead?
                                                    
  std::vector<boost::shared_ptr<boost::thread> > threads;

  DataPlaneOperatorLoader operator_loader;  
  std::map<operator_id_t, boost::shared_ptr<jetstream::DataPlaneOperator> > operators;

  void controller_connected (boost::shared_ptr<ClientConnection> conn,
                             boost::system::error_code error);

  void received_ctrl_msg (boost::shared_ptr<ClientConnection> c,
                          const jetstream::ControlMessage &msg,
                          const boost::system::error_code &error);

  void received_data_msg (boost::shared_ptr<ClientConnection> c,
                          const jetstream::DataplaneMessage &msg,
                          const boost::system::error_code &error);
         
  void incoming_conn_handler(boost::shared_ptr<ConnectedSocket> sock,
                             const boost::system::error_code &);

  
 public:
  Node (const NodeConfig &conf, boost::system::error_code &error);
  ~Node ();

  void start (); //starts and returns after creating threads.
  void stop ();
  
  void join ()  {
    boost::unique_lock<boost::mutex> lock(threadpoolLock);
    startStopCond.wait(lock);
  }

  boost::shared_ptr<DataPlaneOperator> get_operator (operator_id_t name);
  
  operator_err_t
    create_operator (std::string op_typename, operator_id_t, operator_config_t) ;
    
  bool stop_operator (operator_id_t name);
  
  int operator_count () const { 
    boost::unique_lock<boost::mutex> lock(operatorTableLock);
    return operators.size();
  }
  
  const NodeConfig& cfg() { return config;}
    
  boost::shared_ptr<DataCube>
    get_cube (const std::string &name) { return cubeMgr.get_cube(name); }

  const boost::asio::ip::tcp::endpoint & get_listening_endpoint () const
  { return listeningSock->get_local_endpoint(); }

  /* Returns by value; typically the response is very short and so the dynamic alloc
   * overhead isn't worth it.
   */
  ControlMessage handle_alter (ControlMessage& response, const AlterTopo& t);

//TODO include private copy-constructor and operator= here?

};

//const int HB_INTERVAL = 5; //seconds
  
}



#endif /* _node_H_ */
