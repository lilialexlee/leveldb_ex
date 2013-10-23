leveldb_ex
==========
A NoSQL Database based on leveldb


Overview
----------
*   非阻塞网络IO + 多工作线程
  
*   存储引擎使用leveldb

    配置可选的LRU内存cache

*   支持redis通信协议

    可以通过redis自带的客户端redis-cli来连接server进行测试

    支持key/value GET/SET/DEL命令

    支持watch/multi/exec语义的乐观锁,

Run
----------
*   leveldb_ex --flagfile=leveldb_ex.conf
