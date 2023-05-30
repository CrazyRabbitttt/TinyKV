#ifndef TINYKV_SRC_INCLUDE_WAL_FORMAT_
#define TINYKV_SRC_INCLUDE_WAL_FORMAT_

namespace tinykv {
namespace wal {

enum RecordType {
  kZeroType = 0,
  kFullType = 1,
  kFirstType = 2,
  kMiddleType = 3,
  kLastType = 4,
};

static const int kHeaderLen = 7; // 头部字段长度, Fixed 7 bytes
static const int kBlockSize = 32 * 1024;

} // namespace wal
} // namespace tinykv

#endif
