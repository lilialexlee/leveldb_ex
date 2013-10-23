#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>
#include "net/connection.h"
#include "storage/storage.h"

namespace leveldb_ex {
namespace net {

Connection::Connection(boost::asio::io_service& io_service,
                       RequestParser& parser, RequestHandler& handler)
                  : socket_(io_service),
                    strand_(io_service),
                    request_parser_(parser),
                    request_handler_(handler),
                    is_writing_(false),
                    in_multi_state_(false) {
}

Connection::~Connection() {
  ClearWatch();
  LOG(INFO) << "CONN::~CONN " << peer_;
}

boost::asio::ip::tcp::socket& Connection::Socket() {
  return socket_;
}

void Connection::Start() {
  socket_.set_option(boost::asio::socket_base::keep_alive(true));
  socket_.set_option(boost::asio::ip::tcp::no_delay(true));
  StartRead();
}

void Connection::StartRead() {
  boost::asio::detail::socket_option::boolean<IPPROTO_TCP, TCP_QUICKACK> quickack(true);
  socket_.set_option(quickack);
  socket_.async_read_some(
      boost::asio::buffer(buffer_),
      strand_.wrap( boost::bind(
             &Connection::HandleRead,
             shared_from_this(),
             boost::asio::placeholders::error,
             boost::asio::placeholders::bytes_transferred)));
}

void Connection::HandleRead(const boost::system::error_code& e, std::size_t bytes_transferred) {
  if (!e) {
    receive_data_.insert(receive_data_.end(),
        buffer_.data(), buffer_.data() + bytes_transferred);
    while(receive_data_.size() > 0) {
      boost::shared_ptr<Request> request;
      boost::tribool result = request_parser_.Parse(&receive_data_,&request);
      if (result) {
        LOG(INFO) << "REQUEST FROM " << peer_ << " " << request->ToString();
        boost::shared_ptr<Reply> reply = request_handler_.HandleRequest(request,shared_from_this());
        reply->AppendTo(&to_send_data_);
        if( !is_writing_ && to_send_data_.size()>0) {
          StartWrite();
        }
      } else if (!result) {
        socket_.close();
        LOG(WARNING) << "CLOSE CONN cause by invalid request format " << peer_;
        return;
      } else {
        break;
      }
    }
    StartRead();
    return;
  } else {
    socket_.close();
    LOG(WARNING) << "CLOSE CONN caused by read err " << e << " " << peer_;
  }
}

void Connection::StartWrite() {
  is_writing_ = true;
  std::string tmp(to_send_data_.begin(),to_send_data_.end());
  boost::asio::async_write(
      socket_,
      boost::asio::buffer(tmp),
      strand_.wrap( boost::bind(
              &Connection::HandleWrite,
              shared_from_this(),
              boost::asio::placeholders::error)));
  to_send_data_.clear();
}

void Connection::HandleWrite(const boost::system::error_code& e) {
  is_writing_ = false;
  if (e) {
    socket_.close();
    LOG(WARNING) << "CLOSE CONN cause by write err " << e << " " << peer_;
  } else {
    if( to_send_data_.size()>0) {
      StartWrite();
    }
  }
}

void Connection::SetPeer() {
  peer_ = boost::lexical_cast<std::string>(socket_.remote_endpoint());
}

std::string Connection::GetPeer() {
  return peer_;
}

void Connection::Watch(const std::string& key) {
  std::set<std::string>::iterator it = watched_keys_.find(key);
  if( it != watched_keys_.end() ) {
    return;
  } else {
    watched_keys_.insert(key);
    leveldb_ex::storage::GetStorageInstance()->Watch(key,this);
  }
}

void Connection::UnWatch(const std::string& key) {
  std::set<std::string>::iterator it = watched_keys_.find(key);
  if( it == watched_keys_.end() ) {
    return;
  } else {
    watched_keys_.erase(it);
    leveldb_ex::storage::GetStorageInstance()->UnWatch(key,this);
  }
}

void Connection::ClearWatch() {
  for(std::set<std::string>::iterator it = watched_keys_.begin();
    it != watched_keys_.end(); ++it) {
    UnWatch(*it);
  }
  watched_keys_.clear();
}

void Connection::QueueRequest(const boost::shared_ptr<Request>& request) {
  requests_in_queue_.push_back(request);
}

void Connection::ClearQueuedRequest() {
  requests_in_queue_.clear();
}

std::vector<boost::shared_ptr<Request> > Connection::GetQueuedRequest() {
  return requests_in_queue_;
}

} // namespace net
} // namespace leveldb_ex

