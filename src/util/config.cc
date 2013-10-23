#include "util/config.h"

DEFINE_bool(conf_daemonize, true, "run as daemon process");

DEFINE_string(conf_address, "127.0.0.1","server listen address");
DEFINE_string(conf_port, "8800", "server listen port");

DEFINE_int32(conf_threads, 10, "boost asio run threads");

DEFINE_int32(conf_min_log_level, 0, "log severity level, 0-INFO, 1-WARN, 2-ERROR, 3-FATAL");
DEFINE_string(conf_log_dir, "./log", "log directory");
DEFINE_int32(conf_max_log_size, 100, "max log size(in MB), will roll files when exceeds the threshold");

DEFINE_string(conf_leveldb_dir,"./db/", "leveldb directory");

DEFINE_bool(conf_leveldb_create_if_missing, true, "If true, the database will be created if it is missing.");
DEFINE_bool(conf_leveldb_error_if_exits, false, "If true, an error is raised if the database already exists.");
DEFINE_bool(conf_leveldb_paranoid_checks, false, "If true, the implementation will do aggressive checking of the" 
        "data it is processing and will stop early if it detects any errors.");
DEFINE_uint64(conf_leveldb_write_buffer_size, 4<<20, "memtable size");
DEFINE_int32(conf_leveldb_max_open_files, 1000, "Number of open files that can be used by leveldb");
DEFINE_uint64(conf_leveldb_block_size, 4096, "Approximate size of user data packed per block");
DEFINE_uint64(conf_leveldb_block_restart_interval, 16, "Number of keys between restart points for delta encoding of keys.");

DEFINE_bool(conf_leveldb_read_verify_checksums, false, "If true, all data read from underlying storage will be"
        "verified against corresponding checksums.");
DEFINE_bool(conf_leveldb_read_fill_cache, true, "Should the data read for this iteration be cached in memory");

DEFINE_bool(conf_leveldb_write_sync, false, "a DB write with sync==false has similar"
        "crash semantics as the \"write()\" system call.  A DB write"
        "with sync==true has similar crash semantics to a \"write()\""
        "system call followed by \"fsync()\" ");

DEFINE_int32(conf_memory_cache_chunk, 100, "memory cache chunks");
DEFINE_uint64(conf_memory_cache_size, 100000000, "memory cache size");
