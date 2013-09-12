leveldb_ex
==========
A NoSQL Database based on leveldb


说明
----------
*   使用存储引擎基于leveldb
   
    leveldb是Google开源的K/V 存储库，是Bigtable的论文的中的tablet模型的开源实现

    leveldb项目作者是Google Fellow Jeff Dean和Sanjay Ghemawat
    
*   网络通信库使用Boost ASIO，使用单io_service+多线程的模型
  
*   支持redis通信协议

    可以用各种开源的redis客户端来连接server
    
    支持PING 心跳命令
    
    支持key/value的GET/SET/DEL命令 单机SET QPS 10W+

*   使用gflag，参数灵活可配. gflag是goole开源的参数解析工具，可以从命令行传递参数或者使用配置文件

*   使用glog作为日志库. glog是Google开源的C++日志库，轻量易用

*   编译构建工具：gyp. gyp是Google开源的C++编译构建工具，使用在Chromium项目中


编译
----------
*   安装boost 1.54 以上版本
  
*   安装gflags，third_party/gflags-2.0  ./configure; make ; make install
    
*   安装glog，third_party/glog-0.3.3   ./configure; make ; make install

*   编译leveldb，third_party/leveldb-1.13.0  make

*   安装google gyp构建工具. ubuntu: sudo apt-get install gyp

*   编译代码，sh start_build.gyp


运行
----------
*   leveldb_ex --flagfile=leveldb_ex.conf
  
*   参数也可以通过命令行配置，leveldb_ex --help参见帮助，或者打开leveldb_ex.conf查看配置说明

