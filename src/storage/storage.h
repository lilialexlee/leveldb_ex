#ifndef _SOTRAGE_STORAGE_H_
#define _SOTRAGE_STORAGE_H_

#include <map>
#include <boost/thread/mutex.hpp>
#include "leveldb/db.h"

namespace leveldb_ex {
namespace net {
class Connection;
}
}
namespace leveldb_ex {
namespace storage {

class Storage;
Storage* GetStorageInstance();

class Storage {
 public:
  ~Storage();
  leveldb::Status Get(const std::string& key, std::string* value);
  leveldb::Status Set(const std::string& key, const std::string& value);
  leveldb::Status Del(const std::string& key);

  void Watch(const std::string& key, leveldb_ex::net::Connection* const conn);
  void UnWatch(const std::string& key, leveldb_ex::net::Connection* const conn);

  friend void InitStorageInstance();

  bool IsWatchedKeyModified(const std::string& key, leveldb_ex::net::Connection* const conn);

 private:
  Storage();

  void NotifyModifiedKey(const std::string& key);

  leveldb::DB* db_;
  leveldb::Options options_;
  leveldb::WriteOptions write_options_;
  leveldb::ReadOptions read_options_;

  boost::mutex watched_mutex_;
  typedef std::map<leveldb_ex::net::Connection*,bool> WatchClients;
  std::map<std::string,WatchClients > watched_keys_;

  //no copy allowed
  Storage(const Storage&);
  void operator=(const Storage&);
};

}//namespace storage
}//namespace leveldb_ex

#endif
