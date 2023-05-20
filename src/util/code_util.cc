#include "util/code_util.h"

namespace tinykv::util {

void PutFixed32(std::string *dst, uint32_t value) {
  char buf[sizeof(uint32_t)];
  EncodeFixed32(buf, value);
  dst->append(buf, sizeof(value));
}

void PutFixed64(std::string *dst, uint32_t value) {
  char buf[sizeof(uint32_t)];
  EncodeFixed64(buf, value);
  dst->append(buf, sizeof(value));
}

void EncodeFixed32(char *buf, uint32_t value) {
  // 将 value 写进buf 中
  buf[0] = value & 0xff;
  buf[1] = (value >> 8) & 0xff;
  buf[2] = (value >> 16) & 0xff;
  buf[3] = (value >> 24) & 0xff;
}

void EncodeFixed64(char *buf, uint64_t value) {
  buf[0] = value & 0xff;
  buf[1] = (value >> 8) & 0xff;
  buf[2] = (value >> 16) & 0xff;
  buf[3] = (value >> 24) & 0xff;
  buf[4] = (value >> 32) & 0xff;
  buf[5] = (value >> 40) & 0xff;
  buf[6] = (value >> 48) & 0xff;
  buf[7] = (value >> 56) & 0xff;
}

char *EncodeVarint32(char *dst, uint32_t value) {
  // 一个字节的最高位作为【符号位】，
  unsigned char *ptr = reinterpret_cast<unsigned char *>(dst);
  static constexpr int32_t B = 128;
  if (value < (1 << 7)) {
    *(ptr++) = value;
  } else if (value < (1 << 14)) {
    *(ptr++) = value | B;
    *(ptr++) = value >> 7;
  } else if (value < (1 << 21)) {
    *(ptr++) = value | B;
    *(ptr++) = (value >> 7) | B;
    *(ptr++) = value >> 14;
  } else if (value < (1 << 28)) {
    *(ptr++) = value | B;
    *(ptr++) = (value >> 7) | B;
    *(ptr++) = (value >> 14) | B;
    *(ptr++) = value >> 21;
  } else {
    *(ptr++) = value | B;
    *(ptr++) = (value >> 7) | B;
    *(ptr++) = (value >> 14) | B;
    *(ptr++) = (value >> 21) | B;
    *(ptr++) = value >> 28;
  }
  return reinterpret_cast<char *>(ptr);
}

char *EncodeVarint64(char *dst, uint64_t v) {
  static const int B = 128;
  unsigned char *ptr = reinterpret_cast<unsigned char *>(dst);
  while (v >= B) {
    *(ptr++) = (v & (B - 1)) | B;
    v >>= 7;
  }
  *(ptr++) = static_cast<unsigned char>(v);
  return reinterpret_cast<char *>(ptr);
}

inline uint32_t DecodeFixed32(const char *ptr) {
  const uint8_t *buffer = reinterpret_cast<const uint8_t *>(ptr);
  return static_cast<uint32_t>(ptr[0]) | static_cast<uint32_t>(ptr[1] << 8) |
         static_cast<uint32_t>(ptr[2] << 16) |
         static_cast<uint32_t>(ptr[3] << 24);
}

inline uint64_t DecodeFixed64(const char *ptr) {
  const uint8_t *const buffer = reinterpret_cast<const uint8_t *>(ptr);

  // Recent clang and gcc optimize this to a single mov / ldr instruction.
  return (static_cast<uint64_t>(buffer[0])) |
         (static_cast<uint64_t>(buffer[1]) << 8) |
         (static_cast<uint64_t>(buffer[2]) << 16) |
         (static_cast<uint64_t>(buffer[3]) << 24) |
         (static_cast<uint64_t>(buffer[4]) << 32) |
         (static_cast<uint64_t>(buffer[5]) << 40) |
         (static_cast<uint64_t>(buffer[6]) << 48) |
         (static_cast<uint64_t>(buffer[7]) << 56);
}

} // namespace tinykv::util
