#include <fcntl.h>
#include <filesystem>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "test_utils.h"
#include "vfs/localfs.h"

static constexpr size_t SMALL = 128;
static constexpr size_t LARGE = 1024;

class LocalFSTest : public ::testing::Test {
protected:
  std::string basedir;
  std::unique_ptr<LocalFS> fs;

  void SetUp() override {
    basedir = (std::filesystem::temp_directory_path() / generate_random_filename()).string();
    std::filesystem::create_directory(basedir);
    fs = std::make_unique<LocalFS>(basedir);
  }

  void TearDown() override {
    std::filesystem::remove_all(basedir);
  }
};

TEST_F(LocalFSTest, OpenCreatesFile) {
  auto file = fs->open("test.db", O_RDWR);
  ASSERT_TRUE(fs->exists("test.db"));
}

TEST_F(LocalFSTest, WriteAndRead) {
  auto file = fs->open("test.db", O_RDWR);

  std::vector<std::byte> payload = generate_random_payload(SMALL);
  file->write(payload, 0);

  std::vector<std::byte> buf(SMALL);
  file->read(buf, 0);

  ASSERT_EQ(buf, payload);
}

TEST_F(LocalFSTest, WriteAndReadAtOffset) {
  auto file = fs->open("test.db", O_RDWR);

  std::vector<std::byte> payload = generate_random_payload(LARGE);
  file->write(payload, 0);

  off_t offset = 64;
  size_t length = 128;
  std::vector<std::byte> buf(length);
  file->read(buf, offset);

  ASSERT_TRUE(std::equal(buf.begin(), buf.end(), payload.begin() + offset));
}

TEST_F(LocalFSTest, MultipleWrites) {
  auto file = fs->open("test.db", O_RDWR);

  std::vector<std::byte> first = generate_random_payload(SMALL);
  std::vector<std::byte> second = generate_random_payload(SMALL);

  file->write(first, 0);
  file->write(second, SMALL);

  std::vector<std::byte> buf(SMALL);
  file->read(buf, 0);
  ASSERT_EQ(buf, first);

  file->read(buf, SMALL);
  ASSERT_EQ(buf, second);
}

TEST_F(LocalFSTest, SyncDoesNotThrow) {
  auto file = fs->open("test.db", O_RDWR);
  std::vector<std::byte> payload = generate_random_payload(SMALL);
  file->write(payload, 0);
  ASSERT_NO_THROW(file->sync());
}

TEST_F(LocalFSTest, CloseAndReopenPreservesData) {
  std::vector<std::byte> payload = generate_random_payload(SMALL);

  {
    auto file = fs->open("test.db", O_RDWR);
    file->write(payload, 0);
    file->close();
  }

  auto file = fs->open("test.db", O_RDWR);
  std::vector<std::byte> buf(SMALL);
  file->read(buf, 0);
  ASSERT_EQ(buf, payload);
}

TEST_F(LocalFSTest, ExistsFile) {
  ASSERT_FALSE(fs->exists("test.db"));
  auto file = fs->open("test.db", O_RDWR);
  ASSERT_TRUE(fs->exists("test.db"));
}

TEST_F(LocalFSTest, ExistsDirectory) {
  ASSERT_FALSE(fs->exists("subdir"));
  fs->mkdir("subdir");
  ASSERT_TRUE(fs->exists("subdir"));
}

TEST_F(LocalFSTest, MkdirCreatesDirectory) {
  fs->mkdir("subdir");
  ASSERT_TRUE(fs->exists("subdir"));
}

TEST_F(LocalFSTest, RmdirRemovesDirectory) {
  fs->mkdir("subdir");
  fs->rmdir("subdir");
  ASSERT_FALSE(fs->exists("subdir"));
}
