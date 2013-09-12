#ifndef _NET_REQUEST_PARSER_H_
#define _NET_REQUEST_PARSER_H_

#include <deque>
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>
#include "net/packet.h"

namespace leveldb_ex {
namespace net {

class RequestParser {
 public:
  static boost::tribool Parse(std::deque<char>* data,Request* req);
};

} // namespace leveldb_ex
} // namespace net

#endif 

