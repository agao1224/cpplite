#include "storage/filecache.h"
#include "test_utils.h"
#include "vfs/localfs.h"
#include <fcntl.h>
#include <filesystem>
#include <gtest/gtest.h>

class FileCacheTest : public ::testing::Test {
protected:
  std::string basedir;

  void SetUp() override {
    basedir =
        (std::filesystem::temp_directory_path() / generate_random_dirname())
            .string();
    std::filesystem::create_directories(basedir);
  }

  void TearDown() override { std::filesystem::remove_all(basedir); }
};

TEST_F(FileCacheTest, Exists) {
  storage::FileCache cache;
  storage::FileKey k1(0, 0);
  storage::FileKey k2(0, 1);

  std::unique_ptr fs = std::make_unique<LocalFS>(basedir);
  cache.put(k2, fs->open("random1", O_RDWR));
  ASSERT_FALSE(cache.exists(k1));
  ASSERT_TRUE(cache.exists(k2));
}

TEST_F(FileCacheTest, Puts) {
  storage::FileCache cache;
  std::unique_ptr fs = std::make_unique<LocalFS>(basedir);

  size_t num_files = (1 << 5);
  size_t thresh = (1 << 20) - MAX_CACHE_SIZE + 1;
  std::vector<storage::FileKey> expected_keys;

  for (size_t i = 0; i < num_files; i++) {
    storage::FileKey k(1, i);
    const std::string filename = generate_random_filename();
    cache.put(k, fs->open(filename, O_RDWR));
    if (i >= thresh)
      expected_keys.push_back(k);
  }

  for (auto key : expected_keys)
    ASSERT_TRUE(cache.exists(key));
}

TEST_F(FileCacheTest, Gets) {
  storage::FileCache cache;
  std::unique_ptr fs = std::make_unique<LocalFS>(basedir);

  storage::FileKey k1(0, 0);
  storage::FileKey k2(0, 1);
  const std::string f1 = generate_random_filename();
  cache.put(k1, fs->open(f1, O_RDWR));

  VirtualFile *v1 = cache.get(k1);
  ASSERT_NE(v1, nullptr);

  VirtualFile *v2 = cache.get(k2);
  ASSERT_EQ(v2, nullptr);
}

TEST_F(FileCacheTest, EvictLRU) {
  storage::FileCache cache;
  std::unique_ptr fs = std::make_unique<LocalFS>(basedir);

  for (size_t i = 0; i < MAX_CACHE_SIZE; i++) {
    storage::FileKey key(0, i);
    const std::string filename = generate_random_filename();
    cache.put(key, fs->open(filename, O_RDWR));
  }

  for (size_t i = 0; i < MAX_CACHE_SIZE; i++) {
    storage::FileKey key(0, i + MAX_CACHE_SIZE);
    const std::string filename = generate_random_filename();
    cache.put(key, fs->open(filename, O_RDWR));

    storage::FileKey old_key(0, i);
    VirtualFile *old_file = cache.get(old_key);
    ASSERT_FALSE(cache.exists(old_key));
    ASSERT_EQ(old_file, nullptr);
  }
}
