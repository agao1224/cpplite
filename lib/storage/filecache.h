#pragma once

#include "storage_utils.h"
#include "vfs/vfs.h"
#include <map>

constexpr size_t MAX_CACHE_SIZE = 10;

namespace storage {
using FileKey = std::pair<TableID, storage::SegmentID>;

struct FileNode {
  FileKey key;
  std::unique_ptr<VirtualFile> file;
  FileNode *prev;
  FileNode *next;
};

class FileCache {
private:
  std::map<FileKey, FileNode *> filecache_;
  FileNode *head_;
  FileNode *tail_;
  size_t size_ = 0;
  size_t max_size_ = MAX_CACHE_SIZE;

  void insert_node(FileNode *node);
  void delete_node(storage::FileKey key);

public:
  FileCache() : head_(nullptr), tail_(nullptr) {}
  ~FileCache() {
    for (auto &[key, node] : filecache_)
      delete node;
  }

  bool exists(storage::FileKey key);
  void put(storage::FileKey key, std::unique_ptr<VirtualFile> file);
  VirtualFile *get(storage::FileKey key);
};

} // namespace storage
