#include <string>
#include <vector>

#pragma once

std::string generate_random_filename();
std::vector<std::byte> generate_random_payload(size_t num_bytes);
bool check_file_bytes(const std::string filename, std::vector<std::byte> payload);
std::vector<std::byte> get_file_bytes(const std::string filename);