#include "net/packet.h"

namespace leveldb_ex {
namespace net {


namespace misc_strings {

static const char kStatusReplyFlag = '+';
static const char kErrorReplyFlag = '-';
static const char kIntegerReplyFlag = ':';

}


std::string Request::Command() const {
  return datas_.front();
}

size_t Request::ArgNum() const {
  return datas_.size()-1;
}

Request::ConstRandomIterator Request::ArgBegin() const {
  return ++datas_.begin();
}

Request::ConstRandomIterator Request::ArgEnd() const {
  return datas_.end();
}

void Request::Clear() {
  datas_.clear();
}

void Request::AddArg(const std::string& arg) {
  datas_.push_back(arg);
}

void Request::MakeRoomForArgs(size_t arg_num) {
  datas_.reserve(arg_num);
}

std::string Request::ToString() {
  std::ostringstream oss;
  for(size_t i =0; i< datas_.size(); ++i) {
    oss << datas_[i] << " ";
  }
  return oss.str();
}

void Request::CommandToUpper() {
  std::transform(datas_[0].begin(), datas_[0].end(), datas_[0].begin(), toupper);
}

StatusReply::StatusReply(const std::string& status)
            :status_(status) {
}

void StatusReply::AppendTo(std::deque<char>* data) {
  std::ostringstream oss;
  oss << misc_strings::kStatusReplyFlag << status_
      << misc_strings::kCR << misc_strings::kLF;
  std::string tmp(oss.str());
  data->insert(data->end(), tmp.begin(), tmp.end());
}

void StatusReply::AppendTo(MultiBulkReply* mreply){
  std::ostringstream oss;
  oss << misc_strings::kStatusReplyFlag << status_
          << misc_strings::kCR << misc_strings::kLF;
  mreply->AddRawString(oss.str());
}

ErrorReply::ErrorReply(const std::string& error)
            :error_(error) {
}

void ErrorReply::AppendTo(std::deque<char>* data) {
  std::ostringstream oss;
  oss << misc_strings::kErrorReplyFlag << " ERR "
      << error_ << misc_strings::kCR << misc_strings::kLF;
  std::string tmp(oss.str());
  data->insert(data->end(), tmp.begin(), tmp.end());
}

void ErrorReply::AppendTo(MultiBulkReply* mreply){
  std::ostringstream oss;
  oss << misc_strings::kErrorReplyFlag << " ERR "
      << error_ << misc_strings::kCR << misc_strings::kLF;
  mreply->AddRawString(oss.str());
}

IntegerReply::IntegerReply(int num)
            :num_(num) {
}

void IntegerReply::AppendTo(std::deque<char>* data) {
  std::ostringstream oss;
  oss << misc_strings::kIntegerReplyFlag << num_
      << misc_strings::kCR << misc_strings::kLF;
  std::string tmp(oss.str());
  data->insert(data->end(), tmp.begin(), tmp.end());
}

void IntegerReply::AppendTo(MultiBulkReply* mreply){
  std::ostringstream oss;
  oss << misc_strings::kIntegerReplyFlag << num_
      << misc_strings::kCR << misc_strings::kLF;
  mreply->AddRawString(oss.str());
}

BulkReply::BulkReply(const std::string& data)
            :data_(data) {
}

void BulkReply::AppendTo(std::deque<char>* data) {
  if(data_.size() == 0) {
    std::ostringstream oss;
    oss << misc_strings::kArgLenFlag << -1
        << misc_strings::kCR << misc_strings::kLF;
    std::string tmp(oss.str());
    data->insert(data->end(), tmp.begin(), tmp.end());
  } else {
    std::ostringstream oss;
    oss << misc_strings::kArgLenFlag << data_.size()
        << misc_strings::kCR << misc_strings::kLF
        << data_ << misc_strings::kCR << misc_strings::kLF;
    std::string tmp(oss.str());
    data->insert(data->end(), tmp.begin(), tmp.end());
  }
}


void BulkReply::AppendTo(MultiBulkReply* mreply){
  if(data_.size() == 0) {
    std::ostringstream oss;
    oss << misc_strings::kArgLenFlag << -1
        << misc_strings::kCR << misc_strings::kLF;
    mreply->AddRawString(oss.str());
  } else {
    std::ostringstream oss;
    oss << misc_strings::kArgLenFlag << data_.size()
        << misc_strings::kCR << misc_strings::kLF
        << data_ << misc_strings::kCR << misc_strings::kLF;
    mreply->AddRawString(oss.str());
  }
}

void MultiBulkReply::AddRawString(const std::string& str) {
  datas_.push_back(str);
}

void MultiBulkReply::AppendTo(std::deque<char>* data) {
  std::ostringstream oss;
  oss << misc_strings::kArgNumFlag << datas_.size()
      << misc_strings::kCR << misc_strings::kLF;
  std::string tmp(oss.str());
  data->insert(data->end(), tmp.begin(), tmp.end());
  for(size_t i=0; i< datas_.size(); ++i) {
    data->insert(data->end(), datas_[i].begin(), datas_[i].end());
  }
}

void MultiBulkReply::AppendTo(MultiBulkReply* mreply) {
  for(size_t i=0; i< datas_.size(); ++i) {
    mreply->AddRawString(datas_[i]);
  }
}


} // namespace net
} // namespace leveldb_ex
