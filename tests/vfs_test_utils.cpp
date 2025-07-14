#include <random>
#include <string>
#include <chrono>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <gtest/gtest.h>

std::string generate_random_filename() {
  std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());

  const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

  std::uniform_int_distribution<int> length_dist(10, 15);
  int random_length = length_dist(rng);

  std::string random_string;
  std::uniform_int_distribution<int> char_dist(0, characters.length() - 1);
  for (int i = 0; i < random_length; ++i) {
      random_string += characters[char_dist(rng)];
  }
  return "tmp_" + random_string + ".db";
}

std::vector<std::byte> generate_random_payload(size_t num_bytes) {
  std::vector<std::byte> payload(num_bytes);
  std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<int> byte_dist(0, 255);
  for (size_t i = 0; i < num_bytes; ++i) {
    payload[i] = static_cast<std::byte>(byte_dist(rng));
  }
  return payload;
}

bool check_file_bytes(
  const std::string filename,
  std::vector<std::byte> payload
) {
  std::ifstream infile(filename, std::ios::binary);
  if (!infile.is_open()) {
    std::cout << "Failed to open file for reading" << std::endl;
    return false;
  }

  for (size_t i = 0; i < payload.size(); i++) {
    char byte;
    infile.read(&byte, 1);
    if (infile.gcount() != 1) {
      std::cout << "Failed to read byte at index " << i << std::endl;
      return false;
    }
    
    if (payload.at(i) != std::byte{static_cast<unsigned char>(byte)}) {
      std::cout << "Byte mismatch at index " << i << std::endl;
      return false;
    }
  }
  infile.close();
  return true;
}