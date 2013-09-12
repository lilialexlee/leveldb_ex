#include <stdlib.h>
#include <pthread.h>
#include <glog/logging.h>
#include "storage/storage.h"
#include "util/config.h"

namespace leveldb_ex {
namespace storage {

Storage::Storage() {
  options_.create_if_missing = FLAGS_conf_leveldb_create_if_missing;
  options_.error_if_exists = FLAGS_conf_leveldb_error_if_exits;
  options_.paranoid_checks = FLAGS_conf_leveldb_paranoid_checks;
  options_.write_buffer_size = FLAGS_conf_leveldb_write_buffer_size;
  options_.max_open_files = FLAGS_conf_leveldb_max_open_files;
  options_.block_size = FLAGS_conf_leveldb_block_size;
  options_.block_restart_interval = FLAGS_conf_leveldb_block_restart_interval;
  leveldb::Status s = leveldb::DB::Open(options_, FLAGS_conf_leveldb_dir, &db_);
  if(!s.ok()) {
    LOG(ERROR) << "OPEN LEVELDB ERROR " << s.ToString();
    exit(1);
  }
  read_options_.verify_checksums = FLAGS_conf_leveldb_read_verify_checksums;
  read_options_.fill_cache = FLAGS_conf_leveldb_read_fill_cache;
  write_options_.sync = FLAGS_conf_leveldb_write_sync;
}

Storage::~Storage() {
  delete db_;
}

leveldb::Status Storage::Get(const std::string& key, std::string* value) {
  return db_->Get(read_options_, key, value);
}

leveldb::Status Storage::Set(const std::string& key, const std::string& value) {
  return db_->Put(write_options_, key, value);
}

leveldb::Status Storage::Del(const std::string& key) {
  return db_->Delete(write_options_, key);
}

static pthread_once_t once = PTHREAD_ONCE_INIT;

static Storage* instance;

void InitStorageInstance() {
  instance = new Storage;
}

Storage* GetStorageInstance() {
  pthread_once(&once, InitStorageInstance);
  return instance;
}

}//namespace storage
}//namespace leveldb_ex
