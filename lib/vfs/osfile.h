#include <string>
#include <vector>

#pragma once

class OsFile {
  public:
    OsFile(const std::string& path);
    ~OsFile();

    bool os_seek(off_t offset);
    ssize_t os_read(std::vector<std::byte>& buffer, size_t num_bytes);
    bool os_write(std::vector<std::byte> buffer, size_t num_bytes);
    bool os_append(std::vector<std::byte> payload, size_t num_bytes);
    void os_close();
    bool os_open();
    void os_flush();

  private:
    int fd_;
    std::string filename_;
};
