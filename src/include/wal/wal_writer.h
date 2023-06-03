#ifndef TINYKV_SRC_INCLUDE_WAL_WRITER_
#define TINYKV_SRC_INCLUDE_WAL_WRITER_

#include "util/slice.h"
#include "wal/wal_format.h"
#include "util/writable_file.h"
#include "util/status.h"

class WritableFile;

namespace tinykv {
namespace wal {

class Writer {
public:
  Status AddRecord(const Slice& slice);     // add record, the open api exposed to external

private:
  Status DumpRecordToPhysical(RecordType type, const char* ptr, int length);

private:
  int current_block_offset_{0};   // 在当前block的偏移数值
  WritableFile *dest_{nullptr};
  uint32_t crc_check_sum[4+1];
};


}
} // namespace tinykv

#endif
