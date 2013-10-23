#ifndef _SOTRAGE_CACHE_H_
#define _SOTRAGE_CACHE_H_

#include <vector>
#include <string>
#include "util/lru_map.h"

namespace leveldb_ex {
namespace storage {

struct StringHash {
  size_t operator()(const std::string& str) const {
    return __gnu_cxx::hash<const char*>()( str.c_str() );
  }
};

class Cache {
 public:
  Cache(size_t max_size, int chunk_size);
  ~Cache();
  bool LookUp(const std::string& key, std::string* value);
  void Change(const std::string& key, const std::string& value);
  void Remove(const std::string& key);

 private:
  int GetCacheChunk(const std::string& key);

  size_t max_size_;
  int chunk_size_;
  typedef leveldb_ex::util::LRUMap<std::string,std::string,StringHash> CacheChunk;
  std::vector<CacheChunk*> caches_;

  //no copy allowed
  Cache(const Cache&);
  void operator=(const Cache&);
};

}//namespace storage
}//namespace leveldb_ex

#endif
