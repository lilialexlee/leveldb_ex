leveldb_ex
==========
A NoSQL Database based on leveldb


说明
----------
*   使用存储引擎基于leveldb
   
    leveldb是Google开源的K/V 存储库，是Bigtable的论文的中的tablet模型的开源实现

    leveldb项目作者是Google Fellow Jeff Dean和Sanjay Ghemawat
    
*   支持key/value的简单模型,支持GET/SET/DEL等操作  单机QPS可以达到10W+

*   支持check-and-set的乐观锁 

    一个客户端可能的操作:读取一个key的值,做一定的处理,再修改这个key的值. 但在这个过程中可能其他客户端会修改这个key的值.
    可以使用乐观锁来解决这个问题,server会检查这段时间内这个key是否被其他客户端修改过,如果修改过,客户端可以进行重试.
    
    具体参见 http://redis.io/topics/transactions 
  
*   支持redis通信协议, 可以用各种开源的redis客户端来连接server
    
    支持PING 心跳命令
    
    支持key/value的GET/SET/DEL命令

    支持watch/multi/exec语义的乐观锁,通过watch来检查之前的取到的某些key是否被其他客户端修改,来实现乐观锁的高并发
    但multi/exec中的语句不一定是原子执行,因为server与redis不同,是多线程的,CPU中间可能穿插处理其他线程的命令. 

*   网络通信库使用Boost ASIO，使用单io_service+多线程的模型

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

