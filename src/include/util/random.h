#ifndef TINYKV_SRC_INCLUDE_UTIL_RANDOM_
#define TINYKV_SRC_INCLUDE_UTIL_RANDOM_

#include <random>

namespace tinykv {

class RandomNumberGenerator {
public:
  RandomNumberGenerator() : random_engine_(std::random_device()()) {}

  int GenerateRandomNumber(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(random_engine_);
  }
private:
  std::default_random_engine random_engine_;
};

} // namespace tinykv

#endif
