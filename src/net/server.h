#ifndef _NET_SERVER_H_
#define _NET_SERVER_H_

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "net/connection.h"
#include "net/request_parser.h"
#include "net/request_handler.h"

namespace leveldb_ex {
namespace net {

class Server {
 public:
  Server(const std::string& address, const std::string& port, std::size_t thread_pool_size);
  void Run();

private:
  void StartAccept();
  void HandleAccept(const boost::system::error_code& e);
  void HandleStop();

  std::size_t thread_pool_size_;
  boost::asio::io_service io_service_;
  boost::asio::signal_set signals_;
  boost::asio::ip::tcp::acceptor acceptor_;
  ConnectionPtr new_connection_;
  RequestParser request_parser_;
  RequestHandler request_handler_;

  //no copy allowed
  Server(const Server&);
  void operator=(const Server&);
};

} // namespace net
} // namespace leveldb_ex

#endif
