#ifndef TINYKV_SRC_INCLUDE_UTIL_CODE_H
#define TINYKV_SRC_INCLUDE_UTIL_CODE_H

#include <cstdint>
#include <cstring>
#include <string>

namespace tinykv::util {

void PutFixed32(std::string *dst, uint32_t value);
void EncodeFixed32(char *buf, uint32_t value);
void PutFixed64(std::string *dst, uint32_t value);
void EncodeFixed64(char *buf, uint64_t value);
char *EncodeVarint32(char *dst, uint32_t value);
char *EncodeVarint64(char *dst, uint64_t value);
uint32_t DecodeFixed32(const char *ptr);
uint64_t DecodeFixed64(const char *ptr);

} // namespace tinykv::util

#endif