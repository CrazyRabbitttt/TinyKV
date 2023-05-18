#include "memory/alloc.h"

namespace tinykv {



SimpleFreeListAlloc::~SimpleFreeListAlloc() {
    // 需要将内存池的内存全部清除掉, slot中的实际位置其实都是在start_pos之后
    if (!free_list_start_pos_) {
      auto* curnode = (FreeList*)free_list_start_pos_;
      while (curnode) {
        FreeList* nextnode = curnode->next;
        free(curnode);
        curnode = nextnode;
      }
    }
}

int32_t SimpleFreeListAlloc::FreeListIndex(int32_t bytes) {
  // first fit policy, 找到刚刚满足bytes的槽
  return (bytes + kAlignBytes - 1) / kAlignBytes - 1;
}

void *SimpleFreeListAlloc::Allocate(int32_t n) {
  assert(n > 0);
  // 大对象的内存直接使用malloc申请内存
  if (n > kSmallObjectBytes) {
      memory_usage_.fetch_add(n, std::memory_order_relaxed);
      return (char*)malloc(n);
  }
  FreeList **select_free_list{nullptr};
  // find the target slot
  select_free_list = select_free_list + FreeListIndex(n);
  FreeList *result = *(select_free_list);
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
  int32_t real_chunk_number = 10;
  char *alloc_address =
      AllocChunk(bytes, real_chunk_number); // 最终申请到的不一定是10个
  if (real_chunk_number == 1) { // 只申请到了一个chunk，直接返回就好了
    auto *result = reinterpret_cast<FreeList *>(alloc_address);
    return result;
  }
  // 申请的chunk有盈余，将剩余的放到 free list 上

  FreeList *new_free_list{nullptr};
  FreeList *cur{nullptr};
  FreeList *next{nullptr};

  // 留第一个chunk块直接给到user使用
  new_free_list = next = reinterpret_cast<FreeList *>(alloc_address + bytes);
  freelist_[FreeListIndex(bytes)] = new_free_list;
  for (uint32_t curIndex = 1;; ++curIndex) {
    cur = next;
    next = (FreeList *)((char *)next + bytes); // 下一个块的首地址
    if (curIndex != real_chunk_number - 1) {
      cur->next = next;
    } else {
      cur->next = nullptr;
      break;
    }
  }

  // 将首个chunk块返回给用户使用
  auto *result = reinterpret_cast<FreeList *>(alloc_address);
  return result;
}

void SimpleFreeListAlloc::Deallocate(void *address, int32_t size) {
  if (address) {
    auto *p = (FreeList *)address;
    FreeList *volatile *cur_free_list = nullptr;
    memory_usage_.fetch_sub(size, std::memory_order_relaxed);
    if (size > kSmallObjectBytes) {
      free(address); // 大于阈值，直接释放掉
      address = nullptr;
      return;
    }
    cur_free_list = freelist_ + FreeListIndex(size); // 指向对应的slot，然后头插
    p->next = *cur_free_list;
    *cur_free_list = p;
  }
}

void *SimpleFreeListAlloc::ReAllocate(void *address, int curSize, int newSize) {
  Deallocate(address, curSize);
  address = Allocate(newSize);
  return address;
}

char *SimpleFreeListAlloc::AllocChunk(int32_t bytes, int32_t &num) {
  // 期望申请 num 个 bytes 大小的chunk块
  char *result{nullptr};
  int32_t total_bytes = bytes * num;
  uint32_t left_bytes = free_list_end_pos_ - free_list_start_pos_;
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
    return result;
  } else {
    // 剩余的空间连一个chunk块都填充不了，向OS申请
    // 内存池剩余空间一个都没法分配时
    // 在这里又分配了2倍，见uint32_t total_bytes = bytes * nobjs;
    int32_t bytes_to_get = 2 * total_bytes + RoundUp(heap_size_ >> 4);
    if (left_bytes > 0) {
      // 内存池中还有剩余，先配给适当的freelist，否则这部分会浪费掉
      FreeList *volatile *cur_free_list = freelist_ + FreeListIndex(left_bytes);
      // 调整freelist，将内存池剩余空间编入
      ((FreeList *)free_list_start_pos_)->next = *cur_free_list;
      *cur_free_list = ((FreeList *)free_list_start_pos_);
    }

    // 分配新的空间
    free_list_start_pos_ = (char *)malloc(bytes_to_get);
    // 如果分配失败，尝试已经存在的slot能不能装下来
    if (!free_list_start_pos_) {
      FreeList *volatile *my_free_list = nullptr, *p = nullptr;
      // 尝试从freelist中查找是不是有足够大的没有用过的区块
      for (int32_t index = bytes; index <= kSmallObjectBytes;
           index += kAlignBytes) {
        my_free_list = freelist_ + FreeListIndex(index);
        p = *my_free_list;
        if (p) {
          // 说明找到了，此时在进行下一轮循环时候，我们就能直接返回
          *my_free_list = p->next;
          free_list_start_pos_ = (char *)p;
          free_list_end_pos_ = free_list_start_pos_ + index;
          return AllocChunk(bytes, num);
        }
      }
      // 如果未找到，此时我们再重新尝试分配一次，如果分配失败，此时将终止程序
      free_list_end_pos_ = nullptr;
      free_list_start_pos_ = (char *)malloc(bytes_to_get);
      if (!free_list_start_pos_) {
        exit(1);
      }
    }
    heap_size_ += bytes_to_get;
    free_list_end_pos_ = free_list_start_pos_ + bytes_to_get;
    memory_usage_.fetch_add(bytes_to_get, std::memory_order_relaxed);
    return AllocChunk(bytes, num);
  }
}

int32_t SimpleFreeListAlloc::RoundUp(int32_t bytes) {
  return (bytes + kAlignBytes - 1) & ~(kAlignBytes - 1);
}

} // namespace tinykv
