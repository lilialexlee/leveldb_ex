#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>
#include "util/config.h"
#include "net/server.h"

extern void Daemonize();

int main(int argc, char* argv[]) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);
  if(FLAGS_conf_daemonize) {
    Daemonize();
  }

  FLAGS_minloglevel = FLAGS_conf_min_log_level; 
  FLAGS_log_dir = FLAGS_conf_log_dir;
  FLAGS_max_log_size =  FLAGS_conf_max_log_size;

  LOG(ERROR) << "**********PROGRAM START***********";

  try {
    LOG(ERROR) << FLAGS_conf_address;
    LOG(ERROR) << FLAGS_conf_port;
    leveldb_ex::net::Server server(FLAGS_conf_address, FLAGS_conf_port, FLAGS_conf_threads);
    server.Run();
  }
  catch (std::exception& e) {
    LOG(ERROR) << "SERVER EXCEPTION" << e.what();
  }
  return 0;
}

