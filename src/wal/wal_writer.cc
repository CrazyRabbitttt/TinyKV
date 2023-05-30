#include "wal/wal_writer.h"
#include "util/slice.h"
#include "util/writable_file.h"

namespace tinykv {
namespace wal {

void Writer::AddRecord(const Slice &slice) {}

void Writer::DumpRecordToPhysical(RecordType type, const char *ptr,
                                  int length) {
  // 生成record, 交给writable_file具体的进行落盘
  assert(length < 0xffff); // len 不能超过 2bytes
  assert(current_block_offset_ + kHeaderLen + length <= kBlockSize); // 能填充
  char header_buf[7];
  header_buf[4] = static_cast<char>(length & 0xff);
  header_buf[5] = static_cast<char>(length >> 8);
  header_buf[6] = static_cast<char>(type);

  // todo: crc checksum in the first 4 bytes
  //  auto status = dest_.Append(Slice(header_buf));
}

} // namespace wal

} // namespace tinykv
