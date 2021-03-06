#include "connection.h"
#include "jetstream_types.pb.h"

#include <glog/logging.h>

using namespace std;
using namespace boost;
using namespace boost::asio::ip;
using namespace jetstream;


ConnectedSocket::ConnectedSocket (boost::shared_ptr<boost::asio::io_service> srv,
     boost::shared_ptr<boost::asio::ip::tcp::socket> s)
  : iosrv (srv), sock (s), sendStrand (*iosrv), recvStrand(*iosrv), closing_cb(no_op_v),
  sock_state(CS_open),bytesQueued(0),send_counter(NULL), recv_counter(NULL),sending (false), receiving (false) {
  VLOG(1) << "creating connected socket; s " << (s ? "is" : "is not")<< " defined";
  
  std::ostringstream mon_name;
  mon_name << "connection to " << get_remote_endpoint();
  mon = boost::shared_ptr<GenericQCongestionMonitor>(new SmoothingQCongestionMonitor(10 * 1000, mon_name.str()));
  
}



std::string
ConnectedSocket::get_fourtuple () const
{
  boost::system::error_code error_local, error_remote;
  ostringstream fourtuple;

  if (!sock) {
    LOG(FATAL) << "trying to get four-tuple from non-initialized sock";
  }

  tcp::endpoint local = sock->local_endpoint(error_local);
  if (error_local)
    fourtuple << "E";
  else
    fourtuple << local.address().to_string() << ":" << local.port();

  tcp::endpoint remote = sock->remote_endpoint(error_remote);
  if (error_remote) {
    fourtuple << "-E";
  } else
    fourtuple << "-" << remote.address().to_string() << ":" << remote.port();

  return fourtuple.str();
}


void
ConnectedSocket::fail (const boost::system::error_code &error)
{
  //TODO can we remove this logging statement? It's redundant with downstream I think --asr
  if (error != boost::system::errc::operation_canceled)
    LOG(WARNING) << "unexpected error in ConnectedSocket::fail connecting to "<<
        get_remote_endpoint()<< " " << error.message() << endl;

  sending = false;
  sendQueue.clear();
  close_now();

  if (recvcb) {
    SerializedMessageIn bad_msg (0);
    recvcb (bad_msg, error);
  }
}


void
ConnectedSocket::close_now ()
{
  sock_state = CS_closing;
  if (sock->is_open()) {
    boost::system::error_code error;
    sock->cancel(error);
    sock->shutdown(tcp::socket::shutdown_both, error);
    sock->close(error);
  }
  closing_cb();
  VLOG(1) << "callback fired ok";
  sock_state = CS_closed;
}

void
ConnectedSocket::close_on_strand(const close_cb_t& cb) {
  tcp::endpoint endpt = get_remote_endpoint();
  VLOG(1) << "closing connected socket to " << endpt;
  closing_cb = cb; //used in sent() and close_now
  if(!sending) {
    close_now();
    VLOG(1) << "closed connected socket to " << endpt<< "; firing callback";

  } else {
    sock_state = CS_closing;
    VLOG(1) << "socket currently sending; sent() will close";
  }
}

/******************** Sending messages ********************/

ConnectedSocket::SerializedMessageOut::SerializedMessageOut
  (const ProtobufMessage &m, boost::system::error_code &error)
{
  size_t len_check = m.ByteSize();
  if (len_check > MAX_UINT32) {
    error = boost::asio::error::message_size;
    return;
  }
  u_int32_t len = static_cast<u_int32_t> (len_check); // Was lexical_cast. Why? --asr
  u_int32_t len_nbo = htonl (len);

  nbytes = len + hdrlen;
  msg = new u_int8_t[nbytes];

  memcpy(msg, &len_nbo, hdrlen);
  m.SerializeToArray((msg + hdrlen), len);
}


size_t
ConnectedSocket::send_msg (const ProtobufMessage &m,
			   boost::system::error_code &error)
{
  if (!sock->is_open()) {
    error = make_error_code(boost::system::errc::connection_reset);
    return 0;
  }
  if (sock_state != CS_open) {
    LOG(ERROR) << "attempt to send through closed socket"; //programmer error
    error = make_error_code(boost::system::errc::connection_reset);
    return 0;
  }
  boost::shared_ptr<SerializedMessageOut> msg (new SerializedMessageOut (m, error));
  if (!error)
  {
    //LOG(INFO) << "Send queue " << sendQueue.size();
    sendStrand.post(bind(&ConnectedSocket::perform_send, shared_from_this(), msg));
  }
  return msg->nbytes;
}


void
ConnectedSocket::perform_send (boost::shared_ptr<SerializedMessageOut> msg)
{
  if (!sock->is_open())
    return;

  mon->report_insert(msg.get(), msg->nbytes);
  
  if (sending) {
    VLOG(2) << "send is busy in perform_send, queueing" <<endl;

    bytesQueued += msg->nbytes;

    sendQueue.push_back(msg);
    sendStrand.post(bind(&ConnectedSocket::perform_queued_send, 
			  shared_from_this()));
  }
  else {
    sending = true;

    VLOG(2) << "async send in perform_send" <<endl;
    
    mon->report_delete(msg.get(), msg->nbytes);

    // Keep hold of message until callback so not cleaned up until sent
    asio::async_write(*sock, 
		      asio::buffer(msg->msg, msg->nbytes),
		      sendStrand.wrap(bind(&ConnectedSocket::sent, 
					    shared_from_this(), msg, _1, _2)));  }
}

void
ConnectedSocket::perform_queued_send ()
{
  if (sending || sendQueue.empty() || !sock->is_open())
    return;
  VLOG(2) << "queued send" <<endl;
  boost::shared_ptr<SerializedMessageOut> msg = sendQueue.front();
  sendQueue.pop_front();

  sending = true;
  // Keep hold of message until callback so not cleaned up until sent

  if (bytesQueued < msg->nbytes) {
    LOG(FATAL) << "trying to substract "<< msg->nbytes << " from "<<bytesQueued;
  }
  bytesQueued -= msg->nbytes;
  mon->report_delete(msg.get(), msg->nbytes);

  asio::async_write(*sock, 
		    asio::buffer(msg->msg, msg->nbytes),
		    sendStrand.wrap(bind(&ConnectedSocket::sent, 
					 shared_from_this(), msg, _1, _2)));
}


void
ConnectedSocket::sent (boost::shared_ptr<SerializedMessageOut> msg,
		       const boost::system::error_code &error,
		       size_t bytes_transferred)
{
  if(send_counter)
    send_counter->update(bytes_transferred);

  sending = false;

  // XXX Check for specific errors?
  if (error) {
    fail (error);
    return;
  }

  VLOG(2) << "successfully sent "<< bytes_transferred << " bytes" <<endl;
  if (!sendQueue.empty())
    perform_queued_send();
  else {
    if (sock_state == CS_closing) {
      close_now();
    }
  }
}


/******************** Receiving messages ********************/

// Note: This method has to be thread safe.
void
ConnectedSocket::recv_msg (cb_raw_msg_t receivecb)
{
  recvcb = receivecb;
  recvStrand.post(bind(&ConnectedSocket::perform_recv, shared_from_this()));
}


void
ConnectedSocket::perform_recv ()
{

  VLOG(2) << "In ConnectedSocket::perform_recv; port " << sock->local_endpoint().port();
  
  if (!sock->is_open() || receiving)
    return;


  receiving = true;

  // XXX Could also build vector of u_int8_t of size hdrlen, but then we need
  // to still ntohl() from 4 entries of vector.  So, embedding in code for now
  // that hdrlen is u_int32_t
  // // shared_ptr<vector<u_int32_t> > hdrbuf (new vector<u_int8_t> (hdrlen));
  assert(SerializedMessageOut::hdrlen == sizeof (u_int32_t));
  boost::shared_ptr<u_int32_t > hdrbuf (new u_int32_t);

  asio::async_read(*sock,
		   asio::buffer( (void*) hdrbuf.get(), 4),
		   asio::transfer_at_least(SerializedMessageOut::hdrlen),
		   recvStrand.wrap(bind(&ConnectedSocket::received_header,
					 shared_from_this(), hdrbuf, _1, _2)));
}


void
ConnectedSocket::received_header (boost::shared_ptr< u_int32_t > hdrbuf,
				  const boost::system::error_code &error,
				  size_t bytes_transferred)
{
  size_t msglen = 0;

  // XXX Differentiate between temp and permanent errors
  if (error) {
//    LOG(WARNING) << "XXXXXX received_header Failing on error " << error.value() << " " << error.message();  
    receiving = false;
    if (error != boost::system::errc::operation_canceled)
      fail(error); //external close, no need for double-invoke of cleanup
    return;
  }

  boost::system::error_code e;
  if (!hdrbuf 
      || bytes_transferred != SerializedMessageOut::hdrlen)
    e = asio::error::invalid_argument;
  else if (!sock->is_open())
    e = asio::error::not_connected;
  else {
    msglen = ntohl (*hdrbuf);
    if (!msglen)
      e = asio::error::invalid_argument;
  }

  if (e) {
    receiving = false;
    fail(e);
    return;
  }
  VLOG(2) << "In ConnectedSocket::received_header; expecting " << msglen <<
      " data bytes on port " << sock->local_endpoint().port();

  boost::shared_ptr<SerializedMessageIn> recvMsg (new SerializedMessageIn (msglen));

  boost::asio::async_read(*sock,
			  asio::buffer(recvMsg->msg, recvMsg->len),
			  asio::transfer_at_least(recvMsg->len),
			  recvStrand.wrap(bind(&ConnectedSocket::received_body,
						shared_from_this(), recvMsg, _1, _2)));
}


void
ConnectedSocket::received_body (boost::shared_ptr<SerializedMessageIn> recvMsg,
				const boost::system::error_code &error,
				size_t bytes_transferred)

{
  receiving = false;

  // XXX Differentiate between temp and permanent errors
  if (error) {
    fail(error);
    return;
  }
  else if (!recvMsg || bytes_transferred != recvMsg->len) {
    fail(asio::error::invalid_argument);
    return;
  }

  if(recv_counter)
    recv_counter->update(bytes_transferred + 4); //4 for header

  
  VLOG(2) << "In ConnectedSocket::received_body; passing along buffer of length "
     << recvMsg->len << " data bytes on port " << sock->local_endpoint().port();

  if (recvcb) {
    boost::system::error_code success;  //FIXME: where do we set this?
    recvcb(*recvMsg, success);
  }
}


