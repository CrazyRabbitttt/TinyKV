#ifndef TINYKV_SRC_INCLUDE_UTIL_STATUS_H
#define TINYKV_SRC_INCLUDE_UTIL_STATUS_H

namespace tinykv {

enum StatusCode {
  Ok = 0,
  Error = 1,
};

class Status {
public:
  Status() = default;
  explicit Status(const StatusCode& code)
      : status_code_(code) {}
  bool IfOk() { return status_code_ == Ok; }
  bool IfError() { return status_code_ == Error; }
  StatusCode GetStatus() { return status_code_; }

private:
  StatusCode status_code_;
};

} // namespace tinykv

#endif