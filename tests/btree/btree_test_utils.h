#include <memory>
#include <string>
#include <vector>

#include "shared.h"
#include "vfs/osfile.h"
#include "btree/btree.h"

#pragma once

std::vector<std::byte> str_to_bytes(const std::string& s);
void assert_payload(PageNumber record_page, std::shared_ptr<OsFile> db_file_ptr, const std::string& expected);
void assert_cursor_stack(BTreeCursorStack stack, std::vector<BTreeCursorNode> expected_top_to_bottom);
