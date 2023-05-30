#ifndef TINYKV_SRC_INCLUDE_WAL_WRITER_
#define TINYKV_SRC_INCLUDE_WAL_WRITER_

#include "util/slice.h"
#include "wal/wal_format.h"

class WritableFile;

namespace tinykv {
namespace wal {

class Writer {
public:
  void AddRecord(const Slice& slice);     // add record, the open api exposed to external

private:
  void DumpRecordToPhysical(RecordType type, const char* ptr, int length);

private:
  int current_block_offset_{0};   // 在当前block的偏移数值
  WritableFile *dest_{nullptr};
};


}
} // namespace tinykv

#endif
