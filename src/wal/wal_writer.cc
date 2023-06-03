#include "wal/wal_writer.h"
#include "util/code_util.h"
#include "util/slice.h"
#include "util/writable_file.h"

namespace tinykv {
namespace wal {

Status Writer::AddRecord(const Slice &slice) {
  int write_size = slice.size();
  const char *write_data = slice.data();
  bool first_flag = true;
  Status res;
  do {
    // Step1: 检查当前的block是不是还有填充 header 的空间（没有就填充0，用下一块）
    int cur_block_left = kBlockSize - current_block_offset_;
    if (cur_block_left < kHeaderLen) {
      if (cur_block_left > 0) {
        dest_->Append(Slice("\x00\x00\x00\x00\x00\x00", cur_block_left));
      }
      current_block_offset_ = 0;
    }
    assert(kBlockSize - current_block_offset_ >= kHeaderLen);

    int avail = kBlockSize - current_block_offset_ - kHeaderLen;
    size_t fragment_len = std::min(avail, write_size);
    bool end_flag = write_size <= avail;                            // 如果剩余的空间等于本次的写入空间，那么这就是最后一段了

    RecordType type;
    if (first_flag && end_flag) {                                   // 首次写入 & 最后一段 -> Full
      type = kFullType;
    } else if (first_flag) {                                        // 首次写入，但是剩余的空间不够装 -> first type
      type = kFirstType;
    } else if (end_flag) {
      type = kLastType;                                 // 最后一次写入，Last type
    } else {
      type = kMiddleType;
    }

    res = DumpRecordToPhysical(type, write_data, fragment_len);
    write_data += fragment_len;
    write_size -= fragment_len;
    first_flag = false;
  } while (res.IfOk() && write_size > 0);
  return  res;
}

Status Writer::DumpRecordToPhysical(RecordType type, const char *ptr,
                                    int length) {
  // 生成record, 交给writable_file具体的进行落盘
  assert(length < 0xffff); // len 不能超过 2bytes
  assert(current_block_offset_ + kHeaderLen + length <= kBlockSize); // 能填充
  char header_buf[7];
  header_buf[4] = static_cast<char>(length & 0xff);
  header_buf[5] = static_cast<char>(length >> 8);
  header_buf[6] = static_cast<char>(type);
  uint32_t crc_check_sum = 123; // checksum
  // todo : add crc check sum
  util::EncodeFixed32(header_buf, crc_check_sum);
  auto res = dest_->Append(Slice(header_buf));
  if (res.IfOk()) {
    res = dest_->Append(Slice(ptr, length));
    if (res.IfOk()) {
      res = dest_->Flush(); // 将缓冲区中的内容 Flush
                            // 到操作系统的缓冲区中（如果想更稳定就用 fsync
                            // 将缓冲区的数据落盘）
    }
  }
  current_block_offset_ += kHeaderLen + length;
  return res;
}

} // namespace wal

} // namespace tinykv
