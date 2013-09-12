#ifndef _SOTRAGE_STORAGE_H_
#define _SOTRAGE_STORAGE_H_

#include "leveldb/db.h"

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

  friend void InitStorageInstance();

 private:
  Storage();
  leveldb::DB* db_;
  leveldb::Options options_;
  leveldb::WriteOptions write_options_;
  leveldb::ReadOptions read_options_;

  //no copy allowed
  Storage(const Storage&);
  void operator=(const Storage&);
};

}//namespace storage
}//namespace leveldb_ex

#endif
