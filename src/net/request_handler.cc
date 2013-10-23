#include <algorithm>
#include "net/request_handler.h"
#include "net/connection.h"
#include "storage/storage.h"

namespace leveldb_ex {
namespace net {

boost::shared_ptr<Reply> UnkownCommandProc(const boost::shared_ptr<Request>& request,
                boost::shared_ptr<Connection> connection) {
  return boost::shared_ptr<Reply>(
        new ErrorReply(std::string("unknow command ")+request->Command()));
}

boost::shared_ptr<Reply> PingProc(const boost::shared_ptr<Request>& request,
               boost::shared_ptr<Connection> connection) {
  if(request->ArgNum() != 0) {
    return boost::shared_ptr<Reply>(
        new ErrorReply("wrong number of arguments for \'PING\' command"));
  } else {
    return boost::shared_ptr<Reply>(new StatusReply("PONG"));
  }
}

boost::shared_ptr<Reply> SetProc(const boost::shared_ptr<Request>& request,
               boost::shared_ptr<Connection> connection) {
  if(request->ArgNum() != 2) {
    return boost::shared_ptr<Reply>(
        new ErrorReply("wrong number of arguments for \'SET\' command"));
  } else {
    leveldb::Status s = leveldb_ex::storage::GetStorageInstance()->Set(*request->ArgBegin(),*(++request->ArgBegin()));
    if(!s.ok()) {
      return boost::shared_ptr<Reply>(new ErrorReply(s.ToString()));
    }
    return boost::shared_ptr<Reply>(new StatusReply("OK"));
  }
}

boost::shared_ptr<Reply> GetProc(const boost::shared_ptr<Request>& request,
               boost::shared_ptr<Connection> connection) {
  if(request->ArgNum() != 1) {
    return boost::shared_ptr<Reply>(
        new ErrorReply("wrong number of arguments for \'GET\' command"));
  } else {
    std::string value;
    leveldb::Status s = leveldb_ex::storage::GetStorageInstance()->Get(*request->ArgBegin(), &value);
    if(s.IsNotFound()) {
      return boost::shared_ptr<Reply>(new BulkReply(""));
    } else if(!s.ok()) {
      return boost::shared_ptr<Reply>(new ErrorReply(s.ToString()));
    }
    return boost::shared_ptr<Reply>(new BulkReply(value));
  }
}

boost::shared_ptr<Reply> DelProc(const boost::shared_ptr<Request>& request,
               boost::shared_ptr<Connection> connection) {
  if(request->ArgNum() != 1) {
    return boost::shared_ptr<Reply>(
      new ErrorReply("wrong number of arguments for \'DEL\' command"));
  } else {
    leveldb::Status s = leveldb_ex::storage::GetStorageInstance()->Del(*request->ArgBegin());
    if(!s.ok()) {
      return boost::shared_ptr<Reply>(new ErrorReply(s.ToString()));
    }
    return boost::shared_ptr<Reply>(new StatusReply("OK"));
  }
}

boost::shared_ptr<Reply> WatchProc(const boost::shared_ptr<Request>& request,
               boost::shared_ptr<Connection> connection) {
  if(request->ArgNum() != 1) {
    return boost::shared_ptr<Reply>(
        new ErrorReply("wrong number of arguments for \'WATCH\' command"));
  } else {
    connection->Watch(*request->ArgBegin());
    return boost::shared_ptr<Reply>(new StatusReply("OK"));
  }
}

boost::shared_ptr<Reply> UnWatchProc(const boost::shared_ptr<Request>& request,
               boost::shared_ptr<Connection> connection) {
  if(request->ArgNum() != 1) {
    return boost::shared_ptr<Reply>(
        new ErrorReply("wrong number of arguments for \'UNWATCH\' command"));
  } else {
    connection->UnWatch(*request->ArgBegin());
    return boost::shared_ptr<Reply>(new StatusReply("OK"));
  }
}

boost::shared_ptr<Reply> MultiProc(const boost::shared_ptr<Request>& request,
                boost::shared_ptr<Connection> connection) {
  if(request->ArgNum() != 0) {
    return boost::shared_ptr<Reply>(
        new ErrorReply("wrong number of arguments for \'MULTI\' command"));
  } else {
    connection->SetMultiState();
    return boost::shared_ptr<Reply>(new StatusReply("OK"));
  }
}

boost::shared_ptr<Reply> ExecProc(const boost::shared_ptr<Request>& queue,
    boost::shared_ptr<Connection> connection) {
  if (!connection->IsInMultiState()){
    return boost::shared_ptr<Reply>(
        new ErrorReply("EXEC without MULTI"));
  } else {
    std::vector<boost::shared_ptr<Request> > queue = connection->GetQueuedRequest();
    for(size_t i=0; i<queue.size(); ++i) {
      std::string arg = *(queue[i]->ArgBegin());
      if( leveldb_ex::storage::GetStorageInstance()->IsWatchedKeyModified(arg,connection.get()) ) {
        connection->ClearWatch();
        connection->ClearQueuedRequest();
        connection->ResetMultiState();
        return boost::shared_ptr<Reply>(new BulkReply(""));
      }
    }
    MultiBulkReply* result = new MultiBulkReply();
    for(size_t i=0; i<queue.size(); ++i) {
      std::string command = queue[i]->Command();
      std::map<std::string,CommandProc>::iterator it = Commands()->find(command);
      if(it != Commands()->end()) {
        boost::shared_ptr<Reply> reply = it->second(queue[i], connection);
        reply->AppendTo(result);
      } else {
        boost::shared_ptr<Reply> reply = UnkownCommandProc(queue[i], connection);
        reply->AppendTo(result);
      }
    }
    connection->ClearWatch();
    connection->ClearQueuedRequest();
    connection->ResetMultiState();
    return boost::shared_ptr<Reply>(result);
  }
}

static pthread_once_t once = PTHREAD_ONCE_INIT;
static std::map<std::string,CommandProc>* commands;

void InitCommands() {
  commands = new std::map<std::string,CommandProc>();
  commands->insert(std::make_pair("PING",PingProc));
  commands->insert(std::make_pair("SET",SetProc));
  commands->insert(std::make_pair("GET",GetProc));
  commands->insert(std::make_pair("DEL",DelProc));
  commands->insert(std::make_pair("WATCH",WatchProc));
  commands->insert(std::make_pair("UNWATCH",UnWatchProc));
  commands->insert(std::make_pair("MULTI",MultiProc));
  commands->insert(std::make_pair("EXEC",ExecProc));
}

std::map<std::string,CommandProc>* Commands() {
  pthread_once(&once, InitCommands);
  return commands;
}

RequestHandler::RequestHandler() {
}

boost::shared_ptr<Reply> RequestHandler::HandleRequest(const boost::shared_ptr<Request>& request,
    boost::shared_ptr<Connection> connection) {
  std::string command = request->Command();
  if (connection->IsInMultiState()) {
    if( command == "MULTI") {
      return boost::shared_ptr<Reply>(new ErrorReply("MULTI calls can not be nested"));
    } else if (command == "EXEC") {
      return ExecProc(request, connection);
    } else {
      connection->QueueRequest(request);
      return boost::shared_ptr<Reply>(new StatusReply("QUEUED"));
    }
  } else {
    std::map<std::string,CommandProc>::iterator it = Commands()->find(command);
    if(it != Commands()->end()) {
      return it->second(request, connection);
    } else {
      return UnkownCommandProc(request, connection);
    }
  }
}

} // namespace net
} // namespace leveldb_ex

