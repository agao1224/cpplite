#include "filecache.h"
#include "vfs/vfs.h"
#include <cassert>

void storage::FileCache::insert_node(storage::FileNode *node) {
  assert(node != nullptr);
  node->prev = nullptr;
  node->next = nullptr;

  if (head_ == nullptr) {
    assert(tail_ == nullptr);
    head_ = tail_ = node;
  } else {
    node->next = head_;
    head_->prev = node;
    head_ = node;
  }
  return;
}

void storage::FileCache::delete_node(storage::FileKey key) {
  auto it = filecache_.find(key);
  if (it == filecache_.end())
    return;

  FileNode *node = it->second;
  assert(node != nullptr);

  FileNode *prev = node->prev;
  FileNode *next = node->next;
  if (prev != nullptr)
    prev->next = next;
  if (next != nullptr)
    next->prev = prev;

  if (head_ == node)
    head_ = node->next;
  if (tail_ == node)
    tail_ = node->prev;

  filecache_.erase(key);
  return;
}

bool storage::FileCache::exists(storage::FileKey key) {
  return (filecache_.find(key) != filecache_.end());
}

VirtualFile *storage::FileCache::get(storage::FileKey key) {
  auto it = filecache_.find(key);
  if (it == filecache_.end())
    return nullptr;

  storage::FileNode *filenode = it->second;
  assert(filenode != nullptr && filenode->file != nullptr);

  delete_node(key);
  insert_node(filenode);
  filecache_[key] = filenode;
  return (filenode->file).get();
}

void storage::FileCache::put(storage::FileKey key,
                             std::unique_ptr<VirtualFile> file) {
  auto it = filecache_.find(key);
  if (it != filecache_.end()) {
    FileNode *node = it->second;
    delete_node(key);
    insert_node(node);
    return;
  }

  if (size_ == max_size_) {
    FileNode *lru_node = tail_;
    assert(lru_node != nullptr && lru_node->file != nullptr);
    lru_node->file->close();
    FileKey evict_key = lru_node->key;
    delete_node(evict_key);
    delete lru_node;
    size_--;
  }

  FileNode *node = new FileNode{key, std::move(file), nullptr, nullptr};
  insert_node(node);
  size_++;
  filecache_[key] = node;
  return;
}
