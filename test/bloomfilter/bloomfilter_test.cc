#include "filter/bloomfilter.h"

#include <doctest/doctest.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string_view>
#include <cstdint>

#include "util/code_util.h"
#include "util/crc32.h"

using namespace tinykv;
using namespace std;
using namespace tinykv::util;


static const vector<string> kTestKeys = {
    "corekv", "corekv1", "corekv2", "corekv3",
    "corekv4", "corekv5", "corekv6", "core7"
};


TEST_CASE("bloomFilter test") {
  std::unique_ptr<tinykv::BloomFilter> filter_policy = std::make_unique<BloomFilter>(30);
  vector<string> tmp(kTestKeys.begin(), kTestKeys.end());
  filter_policy->CreateFilter(&(tmp[0]), tmp.size());
  std::string src = filter_policy->Data();
  // 将hashnumber填充一下，Match的时候需要读出来
  util::PutFixed32(&src, filter_policy->GetMeta().hash_number_);
  char trailer[5];
  trailer[0] = static_cast<uint8_t>(0);
  ::uint32_t crc = crc32::Value(src.data(), src.size());
  cout << "crc:" << crc << endl;
  tmp.emplace_back("hardcore");
  for (auto& it : tmp) {
    cout << "[ key:" << it
         << ", has_existed:" << filter_policy->MayMatch(it, 0, 0) << " ]"
         << endl;
  }
}





