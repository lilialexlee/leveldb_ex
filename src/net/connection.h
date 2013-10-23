#ifndef _NET_CONNECTION_H_
#define _NET_CONNECTION_H_

#include <deque>
#include <set>
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
  ~Connection();
  boost::asio::ip::tcp::socket& Socket();
  void Start();
  void SetPeer();
  std::string GetPeer();

  void Watch(const std::string& key);
  void UnWatch(const std::string& key);
  void ClearWatch();

  bool IsInMultiState() { return in_multi_state_; }
  void SetMultiState() { in_multi_state_ = true; }
  void ResetMultiState() { in_multi_state_ = false; }

  void QueueRequest(const boost::shared_ptr<Request>& request);
  std::vector<boost::shared_ptr<Request> > GetQueuedRequest();
  void ClearQueuedRequest();

private:
  void StartRead();
  void StartWrite();
  void HandleRead(const boost::system::error_code& e, std::size_t bytes_transferred);
  void HandleWrite(const boost::system::error_code& e);
  void HandleWriteToBadClient(const boost::system::error_code& e);

  boost::asio::ip::tcp::socket socket_;
  boost::asio::io_service::strand strand_;

  RequestParser& request_parser_;
  RequestHandler& request_handler_;

  boost::array<char, 8192> buffer_;
  std::deque<char> receive_data_;
  std::deque<char> to_send_data_;
  bool is_writing_;

  std::string peer_;

  std::set<std::string> watched_keys_;

  bool in_multi_state_;
  std::vector<boost::shared_ptr<Request> > requests_in_queue_;

  //no copy allowed
  Connection(const Connection&);
  void operator=(const Connection&);
};

typedef boost::shared_ptr<Connection> ConnectionPtr;

} // namespace net
} // namespace leveldb_ex

#endif

