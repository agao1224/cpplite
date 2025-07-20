#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "vfs/osfile.h"
#include "test_utils.h"

class VfsTestConstants {
public:
  static constexpr size_t SMALL_BYTES_PAYLOAD = 128;
  static constexpr size_t LARGE_BYTES_PAYLOAD = 1024;
};

TEST(VfsTest, OsFileCreate) {
  const std::string filename = generate_random_filename();
  OsFile os_file(filename);

  ASSERT_TRUE(std::filesystem::exists(filename))
    << "Expected newly created file to exist";
  ASSERT_EQ(std::filesystem::file_size(filename), 0)
    << "Expected newly created file to be of size 0";
}

TEST(VfsTest, OsFileWrite) {
  const std::string filename1 = generate_random_filename();
  OsFile os_file1(filename1);

  std::vector<std::byte> payload1 = generate_random_payload(
    VfsTestConstants::SMALL_BYTES_PAYLOAD
  );
  os_file1.os_write(payload1, VfsTestConstants::SMALL_BYTES_PAYLOAD);

  ASSERT_EQ(
    std::filesystem::file_size(filename1),
    VfsTestConstants::SMALL_BYTES_PAYLOAD
  )
    << "Expected filesize to be 128 bytes";

  ASSERT_TRUE(check_file_bytes(filename1, payload1));

  const std::string filename2 = generate_random_filename();
  OsFile os_file2(filename2);

  std::vector<std::byte> payload2 = generate_random_payload(
    VfsTestConstants::SMALL_BYTES_PAYLOAD
  );
  os_file2.os_write(payload2, 100);

  ASSERT_EQ(std::filesystem::file_size(filename2), 100);

  os_file2.os_close();
  ASSERT_TRUE(os_file2.os_open());

  std::vector<std::byte> payload2_100;
  payload2_100 = std::vector<std::byte>(payload2.begin(), payload2.begin() + 100);
  ASSERT_TRUE(check_file_bytes(filename2, payload2_100));

  std::vector<std::byte> payload3 = generate_random_payload(
    VfsTestConstants::SMALL_BYTES_PAYLOAD
  );
  os_file2.os_write(payload3, VfsTestConstants::SMALL_BYTES_PAYLOAD);

  ASSERT_EQ(
    std::filesystem::file_size(filename2),
    VfsTestConstants::SMALL_BYTES_PAYLOAD
  ) <<
    "Unexpected file size";

  ASSERT_TRUE(check_file_bytes(filename2, payload3));
}

TEST(VfsTest, OsFileAppend) {
  const std::string filename1 = generate_random_filename();
  OsFile os_file1(filename1);

  std::vector<std::byte> payload1 = generate_random_payload(
    VfsTestConstants::SMALL_BYTES_PAYLOAD
  );
  os_file1.os_append(payload1, payload1.size());
  ASSERT_EQ(
    std::filesystem::file_size(filename1), 
    VfsTestConstants::SMALL_BYTES_PAYLOAD
  );
  ASSERT_TRUE(check_file_bytes(filename1, payload1));

  std::vector<std::byte> payload2 = generate_random_payload(
    VfsTestConstants::SMALL_BYTES_PAYLOAD
  );
  os_file1.os_append(payload2, payload2.size());
  payload1.insert(payload1.end(), payload2.begin(), payload2.end());
  ASSERT_TRUE(check_file_bytes(filename1, payload1));
}

TEST(VfsTest, OsFileRead) {
  const std::string filename1 = generate_random_filename();
  OsFile os_file1(filename1);

  std::vector<std::byte> test_payload_1 = generate_random_payload(
    VfsTestConstants::SMALL_BYTES_PAYLOAD
  );
  os_file1.os_write(test_payload_1, test_payload_1.size());

  os_file1.os_close();
  ASSERT_TRUE(os_file1.os_open());

  size_t random_offset = 1 + (std::rand() % VfsTestConstants::SMALL_BYTES_PAYLOAD);
  std::vector<std::byte> read_buffer(random_offset);
  size_t bytes_read = os_file1.os_read(read_buffer, random_offset);

  ASSERT_EQ(bytes_read, random_offset);
  ASSERT_TRUE(std::equal(
    test_payload_1.begin(),
    test_payload_1.begin() + random_offset,
    read_buffer.begin()
  ));
}

TEST(VfsTest, OsFileSeek) {
  const std::string test_db_file = generate_random_filename();
  OsFile test_osfile(test_db_file);

  std::vector<std::byte> test_payload = generate_random_payload(
    VfsTestConstants::LARGE_BYTES_PAYLOAD
  );

  test_osfile.os_write(
    test_payload, VfsTestConstants::LARGE_BYTES_PAYLOAD
  );

  const int NUM_SEEK_TESTS = 50;
  for (int i = 0; i < NUM_SEEK_TESTS; i++) {
    off_t random_offset = 1 + (std::rand() % VfsTestConstants::LARGE_BYTES_PAYLOAD);
    size_t random_size = 1 + (std::rand() % (VfsTestConstants::LARGE_BYTES_PAYLOAD - random_offset - 1));

    std::vector<std::byte> read_buffer(random_size);
    
    test_osfile.os_seek(random_offset);
    test_osfile.os_read(read_buffer, random_size);
    ASSERT_TRUE(std::equal(
      test_payload.begin() + random_offset,
      test_payload.begin() + random_offset + random_size,
      read_buffer.begin()
    ));
  }
}