#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace encoding {

inline void append_uint8(std::vector<std::byte> &buffer, uint8_t val) {
  buffer.push_back(static_cast<std::byte>(val));
}

inline void append_uint16(std::vector<std::byte> &buffer, uint16_t val) {
  for (int i = 1; i >= 0; i--)
    buffer.push_back(static_cast<std::byte>((val >> (i * 8)) & 0xFF));
}

inline void append_uint32(std::vector<std::byte> &buffer, uint32_t val) {
  for (int i = 3; i >= 0; --i)
    buffer.push_back(static_cast<std::byte>((val >> (i * 8)) & 0xFF));
}

inline void append_uint64(std::vector<std::byte> &buffer, uint64_t val) {
  for (int i = 7; i >= 0; i--)
    buffer.push_back(static_cast<std::byte>((val >> (i * 8)) & 0xFF));
}

inline uint8_t read_uint8(const std::vector<std::byte> &buffer,
                          size_t &offset) {
  if (offset + 1 > buffer.size())
    throw std::out_of_range("Buffer underflow reading uint8");
  return static_cast<uint8_t>(buffer[offset++]);
}

inline uint16_t read_uint16(const std::vector<std::byte> &buffer,
                            size_t &offset) {
  if (offset + 2 > buffer.size())
    throw std::out_of_range("Buffer underflow reading uint16");
  uint16_t val = (static_cast<uint16_t>(buffer[offset]) << 8) |
                 (static_cast<uint16_t>(buffer[offset + 1]));
  offset += 2;
  return val;
}

inline uint32_t read_uint32(const std::vector<std::byte> &buffer,
                            size_t &offset) {
  if (offset + 4 > buffer.size())
    throw std::out_of_range("Buffer underflow reading uint32");
  uint32_t val = (static_cast<uint32_t>(buffer[offset]) << 24) |
                 (static_cast<uint32_t>(buffer[offset + 1]) << 16) |
                 (static_cast<uint32_t>(buffer[offset + 2]) << 8) |
                 (static_cast<uint32_t>(buffer[offset + 3]));
  offset += 4;
  return val;
}

inline uint64_t read_uint64(const std::vector<std::byte> &buffer,
                            size_t &offset) {
  if (offset + 8 > buffer.size())
    throw std::out_of_range("Buffer underflow reading uint64");
  uint64_t val = 0;
  for (int i = 0; i < 8; ++i)
    val = (val << 8) | static_cast<uint64_t>(buffer[offset + i]);
  offset += 8;
  return val;
}

} // namespace encoding
