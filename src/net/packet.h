#ifndef _NET_PACKET_H_
#define _NET_PACKET_H_

#include <string>
#include <vector>
#include <deque>
#include <boost/asio.hpp>

namespace leveldb_ex {
namespace net {

class Request {
 public:
  typedef std::vector<std::string>::const_iterator ConstRandomIterator;

  std::string Command() const;
  size_t ArgNum() const;
  ConstRandomIterator ArgBegin() const; 
  ConstRandomIterator ArgEnd() const; 
  void Clear();
  void AddArg(const std::string& arg);
  void MakeRoomForArgs(size_t arg_num);
  std::string ToString();
  
 private:
  std::vector<std::string> datas_;
};

class Reply {
 public:
  virtual void AppendTo(std::deque<char>* data) = 0;
  virtual ~Reply() {};
};

class StatusReply : public Reply {
 public:
  explicit StatusReply(const std::string& status);
  void AppendTo(std::deque<char>* data);
 private:
  std::string status_;
};

class ErrorReply : public Reply {
 public:
  explicit ErrorReply(const std::string& error);
  void AppendTo(std::deque<char>* data);
 private:
  std::string error_;
};

class IntegerReply : public Reply {
 public:
  explicit IntegerReply(int num);
  void AppendTo(std::deque<char>* data);
 private:
  int num_;
};

class BulkReply : public Reply {
 public:
  explicit BulkReply(const std::string& data);
  void AppendTo(std::deque<char>* data);
 private:
  std::string data_;
};

class MultiBulkReply : public Reply {
 public:
  void AppendTo(std::deque<char>* data);
  void AddString(const std::string& str);
 private:
  std::vector<std::string> datas_;
};

static const size_t kMaxLength = 1000;

namespace misc_strings {

static const char kCR = '\r';
static const char kLF = '\n';

static const char kArgNumFlag = '*';
static const char kArgLenFlag = '$';

}

} // namespace net
} // namespace leveldb_ex

#endif 

