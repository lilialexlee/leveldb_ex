#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>
#include "net/connection.h"

namespace leveldb_ex {
namespace net {

Connection::Connection(boost::asio::io_service& io_service,
                       RequestParser& parser, RequestHandler& handler)
                  : strand_(io_service),
                    socket_(io_service),
                    request_parser_(parser),
                    request_handler_(handler),
                    is_writing_(false) {
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
      Request request;
      boost::tribool result = request_parser_.Parse(&receive_data_,&request);
      if (result) {
        LOG(INFO) << "REQUEST FROM " << peer_ << " " << request.ToString();
        boost::shared_ptr<Reply> reply = request_handler_.HandleRequest(request);
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

} // namespace net
} // namespace leveldb_ex

