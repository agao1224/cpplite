#pragma once

#include "vfs/vfs.h"
#include <vector>

class LocalFile : public VirtualFile {
private:
  int fd_;

public:
  LocalFile(int fd) : fd_(fd) {}
  void read(std::vector<std::byte> &buffer, off_t offset);
  void write(std::vector<std::byte> &buffer, off_t offset);
  void sync();
  void close();
  ~LocalFile() = default;
};

class LocalFS : public VFS {
public:
  LocalFS(const std::string &basedir) : VFS(basedir) {}
  std::unique_ptr<VirtualFile> open(const std::string &filename, int flags);
  bool exists(const std::string &path);
  void mkdir(const std::string &dir);
  void rmdir(const std::string &dir);
  ~LocalFS() = default;
};
