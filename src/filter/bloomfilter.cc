#include "filter/bloomfilter.h"
#include "util/hash_util.h"
#include "util/code_util.h"

#include <cmath>

namespace tinykv {

BloomFilter::BloomFilter(uint32_t bits_per_key) : bits_per_key_(bits_per_key) {
  CalcHashNum();
}
void BloomFilter::CalcHashNum() {
  if (bits_per_key_ < 0) {
    bits_per_key_ = 0;
  }
  filter_policy_meta_.hash_number_ = static_cast<int32_t>(
      bits_per_key_ * 0.69314718056); // 0.69314718056 =~ ln(2)
  filter_policy_meta_.hash_number_ = filter_policy_meta_.hash_number_ < 1
                                         ? 1 : filter_policy_meta_.hash_number_;
  filter_policy_meta_.hash_number_ = filter_policy_meta_.hash_number_ > 30
                                         ? 30 : filter_policy_meta_.hash_number_;
}

void BloomFilter::CalcBloomBitsPerKey(int32_t entries_num, float positive) {
  float size = -1 * entries_num * logf(positive) / powf(0.69314718056, 2.0);
  bits_per_key_ = static_cast<int32_t>(ceilf(size / entries_num));
}

const char *BloomFilter::Name() { return "bloom_filter"; }

void BloomFilter::CreateFilter(const std::string *keys, int n) {
  if (n <= 0 || keys == nullptr) {
    return;
  }

  int total_bits = n * bits_per_key_;
  total_bits = total_bits < 64 ? 64 : total_bits;
  const int32_t bytes = (total_bits + 7) / 8;
  total_bits = bytes * 8;

  // 可能多个过滤器共用一个 data_
  const int32_t init_size = bloomfilter_data_.size();
  bloomfilter_data_.resize(init_size + bytes, 0);
  char* array = &(bloomfilter_data_)[init_size];    // 指针从本次的开始位置嘛
  for (int i = 0; i < n; i++) {
    // 使用hash映射到对应的vector中
    uint32_t hash_val = murmurhash::MurMurHash(keys[i].data(), keys[i].size());
    uint32_t delta = (hash_val >> 17) | (hash_val << 15);
    for (size_t j = 0; j < filter_policy_meta_.hash_number_; j++) {
      // 将对应的bit位置为1
      const uint32_t bitpos = hash_val % total_bits;
      array[bitpos/8] |= (1 << (bitpos % 8));
      hash_val += delta;
    }
  }
}


bool BloomFilter::MayMatch(const std::string_view& key, int32_t start_pos,
                           int32_t len) {
  if (key.empty() || bloomfilter_data_.empty()) {
    return false;
  }
  // 转成
  const char* array = bloomfilter_data_.data();
  const size_t total_len = bloomfilter_data_.size();
  if (start_pos >= total_len) {
    return false;
  }
  if (len == 0) {
    len = total_len - start_pos;
  }
  std::string_view bloom_filter(array + start_pos, len);
  const char* cur_array = bloom_filter.data();
  const int32_t bits = len * 8;
  if (filter_policy_meta_.hash_number_ > 30) {
    return true;
  }

  uint32_t hash_val = murmurhash::MurMurHash(key.data(), key.size());
  const uint32_t delta =
      (hash_val >> 17) | (hash_val << 15);  // Rotate right 17 bits
  for (int32_t j = 0; j < filter_policy_meta_.hash_number_; j++) {
    const uint32_t bitpos = hash_val % bits;
    if ((cur_array[bitpos / 8] & (1 << (bitpos % 8))) == 0) {
      return false;
    }
    hash_val += delta;
  }
  return true;
}

bool BloomFilter::MayMatch(const std::string_view& key,
                           const std::string_view& bf_datas) {
  static constexpr uint32_t kFixedSize = 4;
  // 先恢复k_
  const auto& size = bf_datas.size();
  if (size < kFixedSize || key.empty()) {
    return false;
  }
  uint32_t k = util::DecodeFixed32(bf_datas.data() + size - kFixedSize);
  if (k > 30) {
    return true;
  }
  const int32_t bits = (size - kFixedSize) * 8;
  std::string_view bloom_filter(bf_datas.data(), size - kFixedSize);
  const char* cur_array = bloom_filter.data();
  uint32_t hash_val = murmurhash::MurMurHash(key.data(), key.size());
  const uint32_t delta =
      (hash_val >> 17) | (hash_val << 15);  // Rotate right 17 bits
  for (int32_t j = 0; j < k; j++) {
    const uint32_t bitpos = hash_val % bits;
    if ((cur_array[bitpos / 8] & (1 << (bitpos % 8))) == 0) {
      return false;
    }
    hash_val += delta;
  }
  return true;
}














} // namespace tinykv
