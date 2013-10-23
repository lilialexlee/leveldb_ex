#include "storage/cache.h"
#include "util/hash.h"

namespace leveldb_ex  {
namespace storage {

Cache::Cache(size_t max_size, int chunk_size) :
         max_size_(max_size), chunk_size_(chunk_size) {
  caches_.resize(chunk_size_);
  for(int i=0; i<chunk_size_; ++i) {
    caches_[i] = new CacheChunk(max_size_/chunk_size_);
  }
}

Cache::~Cache() {
  for(int i=0; i<chunk_size_; ++i) {
    delete caches_[i];
  }
}

int Cache::GetCacheChunk(const std::string& key) {
  return leveldb_ex::util::hash(key.c_str(),key.size()) % chunk_size_;
}

bool Cache::LookUp(const std::string& key, std::string* value) {
  return caches_[GetCacheChunk(key)]->LookUp(key,value);

}

void Cache::Change(const std::string& key, const std::string& value) {
  caches_[GetCacheChunk(key)]->Change(key,value);
}

void Cache::Remove(const std::string& key) {
  caches_[GetCacheChunk(key)]->Remove(key);
}

}//namespace storage
}//namespace leveldb_ex

