#ifndef TINYKV_SRC_MEMORY_H
#define TINYKV_SRC_MEMORY_H

#include <atomic>
#include <cstdint>
#include <iostream>
#include <cassert>

namespace tinykv {

// Free list node
union FreeList {
  union FreeList *next;
  char data[1];
};

class SimpleFreeListAlloc {
public:
  SimpleFreeListAlloc() {
    std::cout << "Hello from Simple memory pool constructor..\n";
  }
  ~SimpleFreeListAlloc();

  void *Allocate(int32_t n);
  void Deallocate(void *address, int32_t size);

private:
  int32_t Align(int32_t bytes);         // 字节对齐
  int32_t RoundUp(int32_t bytes);       // 向上取整
  static int32_t FreeListIndex(int32_t bytes); // 申请的内存位于哪一个槽
  void *ReFill(int32_t bytes);          // 重新将内存填充到 slot 中
  char *AllocChunk(int32_t bytes, int32_t &num); // 申请num个bytes大小的chunk块
  void *ReAllocate(void *address, int curSize, int newSize);

  uint32_t MemoryUsage() const {
    return memory_usage_.load(std::memory_order_relaxed);
  }

private:
  static constexpr uint32_t kAlignBytes = 8;
  static constexpr uint32_t kSmallObjectBytes = 4096;
  static constexpr uint32_t kFreeListNum = kSmallObjectBytes / kAlignBytes;

  char *free_list_start_pos_{nullptr};
  char *free_list_end_pos_{nullptr};
  int32_t heap_size_{0};
  std::atomic<uint32_t> memory_usage_{0};
  FreeList *freelist_[kFreeListNum] = {nullptr};
};

} // namespace tinykv

#endif
