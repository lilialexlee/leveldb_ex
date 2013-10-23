#include <boost/lexical_cast.hpp>
#include "net/request_parser.h"

namespace leveldb_ex {
namespace net {

boost::tribool RequestParser::Parse(std::deque<char>* data, boost::shared_ptr<Request>* req) {
  //redis protocol
  //see http://redis.io/topics/protocol
  //set mykey myvalue
  //*3\r\n$3\r\nSET\r\n$5\r\nmykey\r\n$7\r\nmyvalue\r\n
  (*req).reset(new Request());
  std::string tt(data->begin(),data->end());
  try {
    (*req)->Clear();
    if(data->front() != misc_strings::kArgNumFlag) return false;
    if(data->size() < 4) return boost::indeterminate;
    if(data->size() > kMaxLength) return false;
    std::deque<char>::iterator it = find(data->begin(), data->end(), misc_strings::kCR);
    if(it == data->end()) return false;
    size_t arg_num = boost::lexical_cast<size_t>(std::string(++data->begin(),it));
    if(arg_num < 0) return false;
    if(++it == data->end()) return boost::indeterminate;
    if(*it != misc_strings::kLF) return false;
    (*req)->MakeRoomForArgs(arg_num);
    while(arg_num > 0) {
      if(++it == data->end()) return boost::indeterminate;
      if(*it != misc_strings::kArgLenFlag) return false;
      if(++it == data->end()) return boost::indeterminate;
      std::deque<char>::iterator tmp = find(it, data->end(), misc_strings::kCR);
      if(tmp == data->end()) return boost::indeterminate;
      size_t arg_len = boost::lexical_cast<size_t>(std::string(it,tmp));
      it = tmp;
      size_t scan_len = it - data->begin();
      if(data->size() < scan_len+2+arg_len+2) return boost::indeterminate;
      if(*(++it) != misc_strings::kLF) return false;
      ++it;
      (*req)->AddArg(std::string(it,it+arg_len));
      it += arg_len;
      if(*it != misc_strings::kCR) return false;
      if(*(++it) != misc_strings::kLF) return false;
      --arg_num;
    }
    data->erase(data->begin(),++it);
    (*req)->CommandToUpper();
    return true;
  } catch(...) {
    return false;
  }
}

} // namespace leveldb_ex
} // namespace net

