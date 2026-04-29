#pragma once

#include <cstdint>
#include <string>
#include <vector>

class VirtualFile {
public:
  virtual void read(std::vector<std::byte> &buffer, size_t len, off_t offset) = 0;
  virtual void write(std::vector<std::byte> *buffer, size_t len, off_t offset) = 0;
  virtual void sync() = 0;
  virtual void close() = 0;
  virtual ~VirtualFile() = default;
};

class VFS {
protected:
  std::string basedir_;

public:
  VFS(const std::string &basedir) : basedir_(basedir) {}
  virtual std::unique_ptr<VirtualFile> open(const std::string &filename,
                                            int flags) = 0;
  virtual bool exists(const std::string &path) = 0;
  virtual bool is_dir(const std::string &path) = 0;
  virtual uint64_t filesize(const std::string &path) = 0;
  virtual std::vector<std::string> ls(const std::string &dir) = 0;
  virtual void mkdir(const std::string &dir) = 0;
  virtual void rmdir(const std::string &dir) = 0;
  virtual void unlink(const std::string &path) = 0;
  virtual ~VFS() = default;
};
