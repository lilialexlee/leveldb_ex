#ifndef _NET_REQUEST_HANDLER_H_
#define _NET_REQUEST_HANDLER_H_

#include <deque>
#include "net/packet.h"

namespace leveldb_ex {
namespace net {

typedef boost::shared_ptr<Reply> (*CommandProc)(const Request& request);

class RequestHandler {
 public:
  RequestHandler();
  boost::shared_ptr<Reply> HandleRequest(const Request& request);

 private:
  std::map<std::string,CommandProc> commands_; 

  //no copy allowed
  RequestHandler(const RequestHandler&);
  void operator=(const RequestHandler&);
};

} // namespace net
} // namespace leveldb_ex

#endif
