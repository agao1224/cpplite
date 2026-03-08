#include <filesystem>
#include <memory>

#include <gtest/gtest.h>
#include "pager/pager.h"
#include "test_utils.h"
#include "vfs/osfile.h"

#pragma once

class DbTestFixture : public ::testing::Test {
protected:
  std::string db_filename;
  std::unique_ptr<Pager> pager;
  std::shared_ptr<OsFile> db_file_ptr;

  void SetUp() override {
    db_filename = generate_random_filename();
    pager = std::make_unique<Pager>(db_filename);
    db_file_ptr = std::make_shared<OsFile>(db_filename);
  }

  void TearDown() override {
    pager.reset();
    std::filesystem::remove(db_filename);
  }
};
