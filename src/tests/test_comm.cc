

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "connection.h"

#define GTEST_HAS_TR1_TUPLE 0
#include <gtest/gtest.h>
#include <iostream>

//using namespace ::std;
using namespace jetstream;
using namespace boost;
using namespace boost::asio::ip;


bool accept_cb_fired;

void my_cb(boost::shared_ptr<ConnectedSocket> sock, const boost::system::error_code &)
{
  accept_cb_fired = true;
}

TEST(Comm, ServerConnection)
{
  // Initialize here in case test is run repeatedly (alternatively create setup
  // and teardown methods for this test case)
  accept_cb_fired = false;
  shared_ptr<asio::io_service> iosrv(new asio::io_service);
  tcp::endpoint local_0(tcp::v4(), 0);
  boost::system::error_code error;
  
  ServerConnection serv_conn(iosrv, local_0, error);
  
  ASSERT_FALSE(accept_cb_fired);
  serv_conn.accept(my_cb, error);
  iosrv->run_one(); //should fire the do_accept callback
  
  tcp::endpoint concrete_local = serv_conn.get_local_endpoint();

  ASSERT_TRUE(serv_conn.is_accepting());
  asio::io_service iosrv2;
  tcp::socket socket(iosrv2);
  boost::system::error_code cli_error;
  socket.connect(concrete_local, cli_error);
  
  iosrv->run_one(); //should fire the accepted callback
  
  //boost::this_thread::sleep(boost::posix_time::seconds(1));
  ASSERT_TRUE(accept_cb_fired);
}

TEST(Comm,PrintPB)
{
  DataplaneMessage data_msg;
  std::ostringstream s;
  s <<"raw message is:" << data_msg.Utf8DebugString() <<std::endl;
}

