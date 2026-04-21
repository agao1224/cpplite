#include "vfs/localfs.h"

#include <fcntl.h>
#include <filesystem>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>

void LocalFile::read(std::vector<std::byte> &buffer, off_t offset) {
  ssize_t bytes_read = pread(fd_, buffer.data(), buffer.size(), offset);
  if (bytes_read == -1)
    throw std::runtime_error("[LocalFile::read]: pread failed");
}

void LocalFile::write(std::vector<std::byte> &buffer, off_t offset) {
  ssize_t bytes_written = pwrite(fd_, buffer.data(), buffer.size(), offset);
  if (bytes_written != static_cast<ssize_t>(buffer.size()))
    throw std::runtime_error("[LocalFile::write]: pwrite failed");
}

void LocalFile::sync() {
  if (fsync(fd_) == -1)
    throw std::runtime_error("[LocalFile::sync]: fsync failed");
}

void LocalFile::close() {
  if (fd_ != -1) {
    ::close(fd_);
    fd_ = -1;
  }
}

std::unique_ptr<VirtualFile> LocalFS::open(const std::string &filename,
                                           int flags) {
  std::string path = basedir_ + "/" + filename;
  int fd = ::open(path.c_str(), flags | O_CREAT, 0644);
  if (fd == -1)
    throw std::runtime_error("[LocalFS::open]: failed to open " + path);

  auto file = std::make_unique<LocalFile>(fd);
  return file;
}

bool LocalFS::exists(const std::string &path) {
  return std::filesystem::exists(basedir_ + "/" + path);
}

void LocalFS::mkdir(const std::string &dir) {
  std::string path = basedir_ + "/" + dir;
  if (::mkdir(path.c_str(), 0755) == -1)
    throw std::runtime_error("[LocalFS::mkdir]: failed to create " + path);
}

void LocalFS::rmdir(const std::string &dir) {
  std::string path = basedir_ + "/" + dir;
  if (::rmdir(path.c_str()) == -1)
    throw std::runtime_error("[LocalFS::rmdir]: failed to remove " + path);
}
