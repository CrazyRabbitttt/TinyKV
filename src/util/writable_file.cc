#include "util/writable_file.h"

namespace tinykv {


CommonWritableFile::CommonWritableFile(const std::string &filename)
    : fstream_(filename, std::fstream::out | std::fstream::trunc),
      file_name_(filename),
      dir_name_(Dirname(filename)) {}

// 使用的是 fstream
// 直接进行数据的写入，能够全部一次性写入（和::write不同，如果说缓冲区满了内部会自动刷盘的）
void CommonWritableFile::DirectWrite(const char *data, size_t size) {
  std::cout << "going to write data(size):" << size << "," <<  data << std::endl;
  fstream_.write(data, size);
}

std::string CommonWritableFile::Dirname(const std::string &filename) {
  auto pos = filename.rfind('/');
  std::string dirname{};
  if (pos != std::string::npos) {
    return filename.substr(0, pos);
  }
  return ".";
}

Status CommonWritableFile::Flush() {
  FlushBuffer();
  return Status(StatusCode::Ok);
}

Status CommonWritableFile::Append(const Slice &data) {
  // 首先将数据写到 buffer 中
  const char *write_data = data.data();
  size_t write_size = data.size();
  size_t minsize = std::min(write_size, kWritableFileBufferSize - cur_pos_);
  std::memcpy(buf_ + cur_pos_, write_data, minsize);
  write_size -= minsize;
  write_data += minsize;
  cur_pos_ += minsize;
  if (write_size == 0) {
    std::cout << "written in buf, the data:" << strlen(buf_) << "," << buf_ << std::endl;
    return Status(StatusCode::Ok);
  }
  // 没写完, 需要刷盘然后判断剩余的是否能够直接放进buffer中
  FlushBuffer();
  if (write_size < kWritableFileBufferSize) {
    std::memcpy(buf_, write_data, write_size);
    cur_pos_ = write_size;
    return Status(StatusCode::Ok);
  }
  // 太大了就直接写
  DirectWrite(write_data, write_size);
  return Status(StatusCode::Ok);
}

Status CommonWritableFile::FlushBuffer() {
  DirectWrite(buf_, cur_pos_);
  cur_pos_ = 0;
  return Status(StatusCode::Ok);
}

Status CommonWritableFile::Close() {
  FlushBuffer();
  fstream_.flush();
  fstream_.close();
  return Status(StatusCode::Ok);
}

Status CommonWritableFile::Fsync() {
  return Status(StatusCode::Ok);
}



}


