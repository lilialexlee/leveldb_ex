#ifndef _NET_CONNECTION_H_
#define _NET_CONNECTION_H_

#include <deque>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "net/packet.h"
#include "net/request_parser.h"
#include "net/request_handler.h"

namespace leveldb_ex {
namespace net {

class Connection : public boost::enable_shared_from_this<Connection> {
 public:
  Connection(boost::asio::io_service& io_service,
             RequestParser& parser, RequestHandler& handler);
  boost::asio::ip::tcp::socket& Socket();
  void Start();
  void SetPeer();
  std::string GetPeer();

private:
  void StartRead();
  void StartWrite();
  void HandleRead(const boost::system::error_code& e, std::size_t bytes_transferred);
  void HandleWrite(const boost::system::error_code& e);
  void HandleWriteToBadClient(const boost::system::error_code& e);

  boost::asio::io_service::strand strand_;
  boost::asio::ip::tcp::socket socket_;

  RequestParser& request_parser_;
  RequestHandler& request_handler_;

  boost::array<char, 8192> buffer_;
  std::deque<char> receive_data_;
  std::deque<char> to_send_data_;
  bool is_writing_;

  std::string peer_;
  
  //no copy allowed
  Connection(const Connection&);
  void operator=(const Connection&);
};

typedef boost::shared_ptr<Connection> ConnectionPtr;

} // namespace net
} // namespace leveldb_ex

#endif 

