#include <cstdint>
#include <cstddef>

#pragma once

typedef uint64_t PageNumber;
const size_t PAGE_SIZE = 4096;
extern const char* DB_FILENAME;
const uint32_t CHECKSUM = 123456;

template<typename T>
struct PagerKey {
  T value;

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
