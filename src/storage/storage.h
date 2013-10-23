#ifndef _SOTRAGE_STORAGE_H_
#define _SOTRAGE_STORAGE_H_

#include <map>
#include <boost/thread/mutex.hpp>
#include "leveldb/db.h"
#include "storage/cache.h"

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
  friend void InitStorageInstance();
  ~Storage();

  leveldb::Status Get(const std::string& key, std::string* value);
  leveldb::Status Set(const std::string& key, const std::string& value);
  leveldb::Status Del(const std::string& key);

  void Watch(const std::string& key, leveldb_ex::net::Connection* conn);
  void UnWatch(const std::string& key, leveldb_ex::net::Connection* conn);
  bool IsWatchedKeyModified(const std::string& key, leveldb_ex::net::Connection* const conn);

 private:
  Storage();

  leveldb_ex::storage::Cache* cache_;

  leveldb::DB* db_;
  leveldb::Options options_;
  leveldb::WriteOptions write_options_;
  leveldb::ReadOptions read_options_;

  const static int kMutexBuckets=1000;
  boost::mutex mutexs_[kMutexBuckets];
  int GetMutexBucket(const std::string& key);

  boost::mutex watched_mutex_;
  typedef std::map<leveldb_ex::net::Connection*,bool> WatchClients;
  std::map<std::string,WatchClients > watched_keys_;
  void NotifyModifiedKey(const std::string& key);

  //no copy allowed
  Storage(const Storage&);
  void operator=(const Storage&);
};

}//namespace storage
}//namespace leveldb_ex

#endif
