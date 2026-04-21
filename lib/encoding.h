#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace encoding {

void append_uint8(std::vector<std::byte> &buffer, uint8_t val);
void append_uint16(std::vector<std::byte> &buffer, uint16_t val);
void append_uint32(std::vector<std::byte> &buffer, uint32_t val);
void append_uint64(std::vector<std::byte> &buffer, uint64_t val);
void append_bool(std::vector<std::byte> &buffer, bool val);
void append_str(std::vector<std::byte> &buffer, const std::string &str);

uint8_t read_uint8(const std::vector<std::byte> &buffer, size_t &offset);
uint16_t read_uint16(const std::vector<std::byte> &buffer, size_t &offset);
uint32_t read_uint32(const std::vector<std::byte> &buffer, size_t &offset);
uint64_t read_uint64(const std::vector<std::byte> &buffer, size_t &offset);
bool read_bool(const std::vector<std::byte> &buffer, size_t &offset);
std::string read_str(const std::vector<std::byte> &buffer, size_t length, size_t &offset);

} // namespace encoding
