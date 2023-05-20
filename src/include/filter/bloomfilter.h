#include "filter/filter_policy.h"

namespace tinykv {

class BloomFilter final : Filter {
public:
  // 直接给定每个Key的bit位数
  BloomFilter(uint32_t bits_per_key);

  BloomFilter(int32_t entries_num, float positive);
  ~BloomFilter() = default;

  const char *Name() override;
  void CreateFilter(const std::string *keys, int n) override;
  bool MayMatch(const std::string_view &key, int start_pos, int n) override;
  bool MayMatch(const std::string_view &key,
                const std::string_view &datas) override;
  const std::string &Data() override { return bloomfilter_data_; }

  const FilterPolicyMeta &GetMeta() override { return filter_policy_meta_; }

  uint32_t Size() override { return bloomfilter_data_.size(); }

private:
  void CalcBloomBitsPerKey(int32_t entries_num, float positive = 0.01);
  void CalcHashNum();

private:
  FilterPolicyMeta filter_policy_meta_{};
  int32_t bits_per_key_{0}; // 每个Key占用的bits数目
  std::string bloomfilter_data_{};
};

} // namespace tinykv
