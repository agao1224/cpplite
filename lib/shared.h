#include <cstdint>
#include <cstddef>
#include <limits>

#pragma once

typedef uint64_t PageNumber;
const size_t PAGE_SIZE = 4096;
extern const char* DB_FILENAME;
const uint32_t CHECKSUM = 123456;

template<typename T>
struct PagerKey {
  T value;
  static constexpr T max_value = std::numeric_limits<T>::max();
  static constexpr T min_value = std::numeric_limits<T>::min();

  PagerKey() = default;
  PagerKey(T v) : value(v) {}

  bool operator==(const PagerKey& o) const { return value == o.value; }
  bool operator!=(const PagerKey& o) const { return value != o.value; }
  bool operator<(const PagerKey& o)  const { return value <  o.value; }
  bool operator>(const PagerKey& o)  const { return value >  o.value; }
  bool operator<=(const PagerKey& o) const { return value <= o.value; }
  bool operator>=(const PagerKey& o) const { return value >= o.value; }
};

using DefaultPagerKey = PagerKey<uint32_t>;
