#ifndef _NET_REQUEST_HANDLER_H_
#define _NET_REQUEST_HANDLER_H_

#include <deque>
#include "net/packet.h"

namespace leveldb_ex {
namespace net {

class Connection;

typedef boost::shared_ptr<Reply> (*CommandProc)(
               const boost::shared_ptr<Request>& request,
               boost::shared_ptr<Connection> connection);

std::map<std::string,CommandProc>* Commands();

class RequestHandler {
 public:
  RequestHandler();
  boost::shared_ptr<Reply> HandleRequest(const boost::shared_ptr<Request>& request,
               boost::shared_ptr<Connection> connection);

 private:
  //no copy allowed
  RequestHandler(const RequestHandler&);
  void operator=(const RequestHandler&);
};

} // namespace net
} // namespace leveldb_ex

#endif
