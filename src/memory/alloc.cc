#include "../include/memory/alloc.h"

namespace tinykv {

int32_t SimpleFreeListAlloc::FreeListIndex(int32_t bytes) {
  // first fit policy, 找到刚刚满足bytes的槽
  return (bytes + kAlignBytes - 1) / kAlignBytes - 1;
}

void *SimpleFreeListAlloc::Allocate(int32_t n) {
  assert(n > 0);
  FreeList **select_free_list{nullptr};
  // find the target slot
  int targetIndex = FreeListIndex(n);
  select_free_list = select_free_list + targetIndex;
  FreeList *result = select_free_list;
  if (result == nullptr) {
    void *ret = (char *)ReFill(RoundUp(n));
    return ret;
  }
  *select_free_list = (*select_free_list)->next; // slot 中去除掉头部的chunk
  return result;
}

void *SimpleFreeListAlloc::ReFill(int32_t bytes) {
  // 分配内存，首先试图分配10个chunk块的内存
  static const int kInitChunkNumber = 10;
  uint32_t real_chunk_number = 10;
  char *alloc_address =
      AllocChunk(bytes, real_chunk_number); // 最终申请到的不一定是10个
  if (real_chunk_number == 1) { // 只申请到了一个chunk，直接返回就好了
    FreeList *result = reinterpret_cast<FreeList *>(alloc_address);
    return result;
  }
  // 申请的chunk有盈余，将剩余的放到 free list 上

  FreeList *new_free_list{nullptr};
  FreeList *cur{nullptr};
  FreeList *next{nullptr};

  // 留第一个chunk块直接给到user使用
  new_free_list = reinterpret_cast<FreeList *>(alloc_address + bytes);
  next = new_free_list
  freelist_[FreeListIndex(bytes)] = new_free_list;
  for (uint32_t curIndex = 1;; ++curIndex) {
    cur = next;
    next = (FreeList*)((char*)next + bytes);  //下一个块的首地址
    if (curIndex != real_block_count - 1) {
      cur->next = next;
    } else {
      cur->next = nullptr;
      break;
    }
  }

  // 将首个chunk块返回给用户使用
  FreeList *result = reinterpret_cast<FreeList *>(alloc_address);
  return result;
}

char *SimpleFreeListAlloc::AllocChunk(int32_t bytes, int32_t &num) {
  // 期望申请 num 个 bytes 大小的chunk块
  char *result{nullptr};
  int32_t total_bytes = bytes * num;
  int32_t left_bytes = free_list_end_pos_ - free_list_start_pos_;
  if (left_bytes >= total_bytes) {
    // 内存池中的剩余部分足够填充 slot
    result = free_list_start_pos_;
    free_list_start_pos_ += total_bytes;
    memory_usage_.fetch_add(total_bytes, std::memory_order_relaxed);
    return result;
  } else if (left_bytes >= bytes) {
    // 虽然说不够填充10个chunk块，但是剩余的能够填充大于一个 bytes 的chunk块
    num = left_bytes / bytes;
    result = free_list_start_pos_;
    free_list_start_pos_ += (num * bytes);
    memory_usage_.fetch_add(num * bytes, std::memory_order_relaxed);
    return result
  } else {
    // 剩余的空间连一个chunk块都填充不了，向OS申请
  }
}

int32_t SimpleFreeListAlloc::RoundUp(int32_t bytes) {
  return (bytes + kAlignBytes - 1) & ~(kAlignBytes - 1);
}

} // namespace tinykv
