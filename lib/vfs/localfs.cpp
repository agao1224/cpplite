#include "vfs/localfs.h"

#include <fcntl.h>
#include <filesystem>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>

void LocalFile::read(std::vector<std::byte> &buffer, size_t len, off_t offset) {
  if (buffer.size() < len)
    buffer.resize(len);
  ssize_t bytes_read = pread(fd_, buffer.data(), len, offset);
  if (bytes_read == -1)
    throw std::runtime_error("[LocalFile::read]: pread failed");
}

void LocalFile::write(std::vector<std::byte> *buffer, size_t len, off_t offset) {
  std::vector<std::byte> padding;
  if (buffer == nullptr) {
    padding.resize(len, std::byte{0});
    buffer = &padding;
  } else if (buffer->size() < len) {
    buffer->resize(len, std::byte{0});
  }
  ssize_t bytes_written = pwrite(fd_, buffer->data(), len, offset);
  if (bytes_written != static_cast<ssize_t>(len))
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

std::vector<std::string> LocalFS::ls(const std::string &dir) {
  std::string path = basedir_ + "/" + dir;
  std::vector<std::string> entries;
  for (const auto &entry : std::filesystem::directory_iterator(path))
    entries.push_back(entry.path().string());
  return entries;
}

uint64_t LocalFS::filesize(const std::string &path) {
  std::string full_path = basedir_ + "/" + path;
  if (std::filesystem::is_directory(full_path))
    throw std::runtime_error("[LocalFS::filesize]: path is a directory: " + path);
  return static_cast<uint64_t>(std::filesystem::file_size(full_path));
}

bool LocalFS::is_dir(const std::string &path) {
  return std::filesystem::is_directory(basedir_ + "/" + path);
}

bool LocalFS::exists(const std::string &path) {
  return std::filesystem::exists(basedir_ + "/" + path);
}

void LocalFS::mkdir(const std::string &dir) {
  std::string path = basedir_ + "/" + dir;
  if (::mkdir(path.c_str(), 0755) == -1)
    throw std::runtime_error("[LocalFS::mkdir]: failed to create " + path);
}

void LocalFS::unlink(const std::string &path) {
  if (::unlink((basedir_ + "/" + path).c_str()) == -1)
    throw std::runtime_error("[LocalFS::unlink]: failed to remove " + path);
}

void LocalFS::rmdir(const std::string &dir) {
  std::string path = basedir_ + "/" + dir;
  if (::rmdir(path.c_str()) == -1)
    throw std::runtime_error("[LocalFS::rmdir]: failed to remove " + path);
}
