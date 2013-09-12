#ifndef _UTIL_CONFIG_H_
#define _UTIL_CONFIG_H_

#include <gflags/gflags.h>

DECLARE_bool(conf_daemonize);

DECLARE_string(conf_address);
DECLARE_string(conf_port);

DECLARE_int32(conf_threads);

DECLARE_int32(conf_min_log_level);
DECLARE_string(conf_log_dir);
DECLARE_int32(conf_max_log_size);

DECLARE_string(conf_leveldb_dir);

DECLARE_bool(conf_leveldb_create_if_missing);
DECLARE_bool(conf_leveldb_error_if_exits);
DECLARE_bool(conf_leveldb_paranoid_checks);
DECLARE_uint64(conf_leveldb_write_buffer_size);
DECLARE_int32(conf_leveldb_max_open_files);
DECLARE_uint64(conf_leveldb_block_size);
DECLARE_uint64(conf_leveldb_block_restart_interval);

DECLARE_bool(conf_leveldb_read_verify_checksums);
DECLARE_bool(conf_leveldb_read_fill_cache);

DECLARE_bool(conf_leveldb_write_sync);

#endif
