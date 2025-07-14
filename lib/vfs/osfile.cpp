#include "osfile.h"
#include "../shared.h"
#include <cstddef>
#include <sys/fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>


OsFile::OsFile(const std::string& path) {
  fd_ = open(path.c_str(), O_RDWR | O_CREAT, 0644);
  if (fd_ == -1)
    perror("[VFS/New]: Failed to open DB file");
  else
    filename_ = path;
}

OsFile::~OsFile() {
  if (fd_ != -1)
    close(fd_);
}

bool OsFile::os_seek(off_t offset) {
  if (fd_ == -1) {
    perror("[VFS/Seek]: Unable to seek in DB file");
    return false;
  }

  off_t bytes_offset = lseek(fd_, offset, SEEK_SET);
  if (bytes_offset == -1) {
    perror("[VFS/Seek]: Error seeking in DB file");
    return false;
  }
  return true;
}

ssize_t OsFile::os_read(std::vector<std::byte>& buffer, size_t num_bytes) {
  if (fd_ == -1) {
    perror("[VFS/Read]: Unable to open DB file");
    return -1;
  }

  ssize_t bytes_read = read(fd_, buffer.data(), num_bytes);
  std::cout << "bytes read: " << bytes_read << std::endl;
  if (bytes_read == -1) {
    perror("[VFS/Read]: Error reading bytes from DB file");
    return -1;
  }
  return bytes_read;
}

bool OsFile::os_write(
  std::vector<std::byte> bytes_to_write,
  size_t num_bytes
) {
  if (fd_ == -1) {
    perror("[VFS/Write]: Unable to open DB file");
    return false;
  }

  ssize_t bytes_written = write(fd_, bytes_to_write.data(), num_bytes);
  if (bytes_written != num_bytes) {
    fprintf(stderr, "[VFS/Write]: Failed to write %zu bytes (wrote %zd)\n", num_bytes, bytes_written);
    return false;
  }
  return true;
}

bool OsFile::os_append(std::vector<std::byte> payload, size_t num_bytes) {
  if (fd_ == -1) {
    perror("[VFS/Append]: Invalid file descriptor");
    return false;
  }

  if (lseek(fd_, 0, SEEK_END) == -1) {
    perror("[VFS/Append]: Failed to seek to end");
    return false;
  }

  if (num_bytes > payload.size())
    payload.insert(payload.end(), num_bytes - payload.size(), std::byte{0});

  ssize_t bytes_written = write(fd_, payload.data(), num_bytes);
  if (bytes_written != static_cast<ssize_t>(num_bytes)) {
    fprintf(stderr, "[VFS/Append]: Failed to write %zu bytes (wrote %zd)\n", num_bytes, bytes_written);
    return false;
  }

  return true;
}

void OsFile::os_close() {
  if (fd_ == -1) {
    perror("[VFS/Append]: Invalid file descriptor");
    return;
  }
  close(fd_);
}

bool OsFile::os_open() {
  fd_ = open(filename_.c_str(), O_RDWR, 0644);
  if (fd_ == -1) {
    perror("[VFS/New]: Failed to open DB file");
    return false;
  }
  return true;
}

void OsFile::os_flush() {
  if (fsync(fd_) == -1) {
    perror("[VFS/Flush]: Error flushing DB file");
  }
  close(fd_);
}