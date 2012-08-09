#ifndef _workerclient_H_
#define _workerclient_H_

#include <sys/types.h>
#include <boost/asio.hpp>

#include "js_utils.h"
#include "jetstream_types.pb.h"
#include "jetstream_dataplane.pb.h"

using namespace ::google;
using namespace boost::asio::ip;


namespace jetstream {

class WorkerConnHandler {
  public:
 WorkerConnHandler(boost::asio::io_service& io_service,
      tcp::resolver::iterator endpoint_iterator);

 void write(const protobuf::Message &msg);
 void close();

 virtual void processMessage(protobuf::Message &msg) = 0;


private:

  void handle_connect(const boost::system::error_code& error);
  void handle_read_header(const boost::system::error_code& error);

  void handle_read_body(const boost::system::error_code& error);

//  void do_write(chat_message msg);

  void handle_write(const boost::system::error_code& error);

  void do_close();

private:
  boost::asio::io_service& io_service_;
  tcp::socket socket_;
};

}

#endif