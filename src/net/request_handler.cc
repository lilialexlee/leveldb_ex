#include <algorithm>
#include "net/request_handler.h"
#include "storage/storage.h"

namespace leveldb_ex {
namespace net {

boost::shared_ptr<Reply> PingProc(const Request& request) {
  if(request.ArgNum() != 0) {
    return boost::shared_ptr<Reply>(
        new ErrorReply("wrong number of arguments for \'PING\' command"));
  } else {
    return boost::shared_ptr<Reply>(new StatusReply("PONG"));
  }
}

boost::shared_ptr<Reply> SetProc(const Request& request) {
  if(request.ArgNum() != 2) {
    return boost::shared_ptr<Reply>(
        new ErrorReply("wrong number of arguments for \'SET\' command"));
  } else {
    leveldb::Status s = leveldb_ex::storage::GetStorageInstance()->Set(*request.ArgBegin(),*(++request.ArgBegin()));
    if(!s.ok()) { 
      return boost::shared_ptr<Reply>(new ErrorReply(s.ToString()));
    }
    return boost::shared_ptr<Reply>(new StatusReply("OK"));
  }
}

boost::shared_ptr<Reply> GetProc(const Request& request) {
  if(request.ArgNum() != 1) {
    return boost::shared_ptr<Reply>(
        new ErrorReply("wrong number of arguments for \'GET\' command"));
  } else {
    std::string value;
    leveldb::Status s = leveldb_ex::storage::GetStorageInstance()->Get(*request.ArgBegin(), &value);
    if(s.IsNotFound()) { 
      return boost::shared_ptr<Reply>(new BulkReply(""));
    } else if(!s.ok()) { 
      return boost::shared_ptr<Reply>(new ErrorReply(s.ToString()));
    }
    return boost::shared_ptr<Reply>(new BulkReply(value));
  }
}

boost::shared_ptr<Reply> DelProc(const Request& request) {
  if(request.ArgNum() != 1) {
    return boost::shared_ptr<Reply>(
        new ErrorReply("wrong number of arguments for \'DEL\' command"));
  } else {
    leveldb::Status s = leveldb_ex::storage::GetStorageInstance()->Del(*request.ArgBegin());
    if(!s.ok()) { 
      return boost::shared_ptr<Reply>(new ErrorReply(s.ToString()));
    }
    return boost::shared_ptr<Reply>(new StatusReply("OK"));
  }
}

boost::shared_ptr<Reply> UnkownCommandProc(const Request& request) {
  return boost::shared_ptr<Reply>(
        new ErrorReply(std::string("unknow command ")+request.Command()));
}

RequestHandler::RequestHandler() {
  commands_.insert(std::make_pair("PING",PingProc));  
  commands_.insert(std::make_pair("SET",SetProc));  
  commands_.insert(std::make_pair("GET",GetProc));  
  commands_.insert(std::make_pair("DEL",DelProc));  
}

boost::shared_ptr<Reply> RequestHandler::HandleRequest(const Request& request) {
  std::string command = request.Command();
  std::transform(command.begin(), command.end(), command.begin(), toupper);
  std::map<std::string,CommandProc>::iterator it = commands_.find(command);
  if(it != commands_.end()) {
    return it->second(request);
  } else {
    return UnkownCommandProc(request);
  }

}

} // namespace net
} // namespace leveldb_ex

