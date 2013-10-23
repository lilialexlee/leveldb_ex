#ifndef _UTIL_LRU_MAP_H_
#define _UTIL_LRU_MAP_H_

#include <list>
#include <ext/hash_map>
#include <boost/thread/mutex.hpp>

namespace leveldb_ex {
namespace util {

template<typename K, typename V, typename HashFcn>
class LRUMap {
 public:
  typedef K KeyType;
  typedef V ValueType;
  typedef std::list<KeyType> VisitTracker;
  typedef __gnu_cxx::hash_map<KeyType, std::pair<ValueType, typename VisitTracker::iterator>, HashFcn> MapType;

  LRUMap(size_t max_size) :max_size_(max_size) {
  }

  bool LookUp(const KeyType& key, ValueType* value) {
    boost::mutex::scoped_lock lock(mutex_);
    typename MapType::iterator it = map_.find(key);
    if ( it != map_.end() ) {
      //move the entry to the end of track list
      *value = (it->second).first;
      visit_tracker_.splice( visit_tracker_.end(), visit_tracker_, (it->second).second );
      return true;
    }
    return false;
  }

  void Change(const KeyType& key,const ValueType& value) {
    boost::mutex::scoped_lock lock(mutex_);
    typename MapType::iterator it = map_.find(key);
    if( it != map_.end() ) {
      it->second.first = value;
    } else {
      if ( map_.size() >= max_size_ ) {
        Evict();
      }
      visit_tracker_.push_back(key);
      map_.insert(std::make_pair(key, std::make_pair(value, --visit_tracker_.end())));
    }
  }

  void Remove(const KeyType& key) {
    boost::mutex::scoped_lock lock(mutex_);
    typename MapType::iterator it = map_.find(key);
    if ( it != map_.end() ) {
      visit_tracker_.erase( (it->second).second );
      map_.erase(it);
    }
  }

  size_t Size() {
    boost::mutex::scoped_lock lock(mutex_);
    return map_.size();
  }

 private:
  void Evict() {
    typename MapType::iterator it = map_.find( visit_tracker_.front() );
    map_.erase(it);
    visit_tracker_.pop_front();
  }

  boost::mutex mutex_;
  size_t max_size_;
  //for least visit list, the last one is the latest visited one
  VisitTracker visit_tracker_;
  MapType map_;
};

}}//namespace

#endif
