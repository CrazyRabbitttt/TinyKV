#ifndef TINYKV_SRC_INCLUDE_UTIL_WRITABLEFILE_
#define TINYKV_SRC_INCLUDE_UTIL_WRITABLEFILE_

#include <algorithm>
#include <fstream>
#include <iostream>

#include "util/slice.h"
#include "util/status.h"

namespace tinykv {

// basic class
class WritableFile {
public:
  WritableFile() = default;

  WritableFile(const WritableFile &) = delete;
  WritableFile &operator=(const WritableFile &) = delete;
  virtual ~WritableFile()=default;
  virtual Status Append(const Slice &data) = 0;
  virtual Status Flush() = 0;
  virtual Status Close() = 0;
  virtual Status Fsync() = 0;
};

constexpr const int kWritableFileBufferSize = 64 * 1024;

class CommonWritableFile : public WritableFile {
public:
  CommonWritableFile() = default;
  explicit CommonWritableFile(const std::string &filename);
  ~CommonWritableFile() override {
    fstream_.flush();
    fstream_.close();
  }

  Status Append(const Slice &data) override;
  Status Flush() override;
  Status Close() override;
  Status Fsync() override;

private:
  Status FlushBuffer();
  void DirectWrite(const char *data, size_t size);
  std::string Dirname(const std::string& filename);

private:
  char buf_[kWritableFileBufferSize]{};
  size_t cur_pos_{0};
  std::fstream fstream_;
  std::string file_name_{};
  std::string dir_name_{};
};

} // namespace tinykv

#endif