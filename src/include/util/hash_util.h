#ifndef TINYKV_SRC_INCLUDE_UTIL_HASH_
#define TINYKV_SRC_INCLUDE_UTIL_HASH_

#include <cstdint>

namespace tinykv::murmurhash {

uint32_t MurMurHash(const char *data, uint32_t len);

} // namespace tinykv::murmurhash

#endif