#ifndef TINYKV_SRC_INCLUDE_UTIL_WRITABLEFILE_
#define TINYKV_SRC_INCLUDE_UTIL_WRITABLEFILE_

class Slice;

namespace tinykv {

// basic class
class WritableFile {
public:
  WritableFile() = default;

  WritableFile(const WritableFile &) = delete;
  WritableFile &operator=(const WritableFile &) = delete;

  virtual ~WritableFile();

  virtual void Append(const Slice &data) = 0;
  virtual void Flush() = 0;
  virtual void Close() = 0;
  virtual void Fsync() = 0;
};

} // namespace tinykv

#endif