#include "db_test_fixture.h"
#include "encoding.h"
#include "record/record.h"
#include "schema.h"
#include "gtest/gtest.h"

#include <variant>
#include <vector>

class RecordManagerTest : public DbTestFixture {};

TEST_F(RecordManagerTest, ValueSerializerBasic) {
  record::Row row;
  row.values.push_back(5);
  row.values.push_back("foobar");
  row.values.push_back(true);

  std::vector<std::byte> buffer;
  record::ValueSerializer visitor{buffer};
  std::visit(visitor, row.values[0]);
  std::visit(visitor, row.values[1]);
  std::visit(visitor, row.values[2]);

  size_t offset = 0;
  int64_t test_int =
      static_cast<int64_t>(encoding::read_uint64(buffer, offset));
  ASSERT_EQ(test_int, 5);

  uint32_t text_length = encoding::read_uint32(buffer, offset);
  std::string test_str = encoding::read_str(buffer, text_length, offset);
  EXPECT_EQ(test_str, "foobar");

  bool test_bool = encoding::read_bool(buffer, offset);
  ASSERT_TRUE(test_bool);
}

TEST_F(RecordManagerTest, SerializeBasic) {
  record::Row row;
  row.values.push_back(1234);
  row.values.push_back("foo@bar.com");
  row.values.push_back(true);
  row.values.push_back(15445);
  row.values.push_back("baz");

  schema::Table table;
  table.name = "testing";
  table.type = schema::SchemaType::TABLE;
  table.tbl_name = "testing";

  table.columns.push_back({
      .type = schema::DataType::INTEGER,
      .name = "id",
      .is_primary_key = true,
      .is_nullable = false,
  });
  table.columns.push_back({
      .type = schema::DataType::TEXT,
      .name = "email",
      .is_primary_key = false,
      .is_nullable = false,
  });
  table.columns.push_back({
      .type = schema::DataType::BOOLEAN,
      .name = "is_registered",
      .is_primary_key = false,
      .is_nullable = false,
  });
  table.columns.push_back({
      .type = schema::DataType::INTEGER,
      .name = "course_code",
      .is_primary_key = false,
      .is_nullable = false,
  });
  table.columns.push_back({
      .type = schema::DataType::TEXT,
      .name = "notes",
      .is_primary_key = false,
      .is_nullable = false,
  });

  RecordManager record;
  std::vector<std::byte> buffer = record.serialize(row, table);
  size_t offset = 0;
  uint64_t xmin = encoding::read_uint64(buffer, offset);
  uint64_t xmax = encoding::read_uint64(buffer, offset);
  uint16_t flags = static_cast<uint16_t>(encoding::read_uint16(buffer, offset));
  ASSERT_EQ(xmin, 0);
  ASSERT_EQ(xmax, 0);
  ASSERT_EQ(flags, 0x0000);

  int64_t id = static_cast<int64_t>(encoding::read_uint64(buffer, offset));
  ASSERT_EQ(id, 1234);

  uint32_t email_length = encoding::read_uint32(buffer, offset);
  ASSERT_EQ(email_length, sizeof("foo@bar.com") - 1);
  std::string email = encoding::read_str(buffer, email_length, offset);
  EXPECT_EQ(email, "foo@bar.com");

  bool is_registered = encoding::read_bool(buffer, offset);
  ASSERT_TRUE(is_registered);

  int64_t course_code =
      static_cast<int64_t>(encoding::read_uint64(buffer, offset));
  ASSERT_EQ(course_code, 15445);

  uint32_t notes_length = encoding::read_uint32(buffer, offset);
  ASSERT_EQ(notes_length, sizeof("baz") - 1);
  std::string notes = encoding::read_str(buffer, notes_length, offset);
  EXPECT_EQ(notes, "baz");

  ASSERT_EQ(offset, buffer.size());
}

TEST_F(RecordManagerTest, SerializeNulls) {
  record::Row row;
  row.values.push_back(1234);
  row.values.push_back("foo@bar.com");
  row.values.push_back(record::Null{});
  row.values.push_back(record::Null{});
  row.values.push_back("baz");

  schema::Table table;
  table.name = "testing";
  table.type = schema::SchemaType::TABLE;
  table.tbl_name = "testing";

  table.columns.push_back({
      .type = schema::DataType::INTEGER,
      .name = "id",
      .is_primary_key = true,
      .is_nullable = false,
  });
  table.columns.push_back({
      .type = schema::DataType::TEXT,
      .name = "email",
      .is_primary_key = false,
      .is_nullable = false,
  });
  table.columns.push_back({
      .type = schema::DataType::BOOLEAN,
      .name = "is_registered",
      .is_primary_key = false,
      .is_nullable = true,
  });
  table.columns.push_back({
      .type = schema::DataType::INTEGER,
      .name = "course_code",
      .is_primary_key = false,
      .is_nullable = true,
  });
  table.columns.push_back({
      .type = schema::DataType::TEXT,
      .name = "notes",
      .is_primary_key = false,
      .is_nullable = false,
  });

  RecordManager record;
  std::vector<std::byte> buffer = record.serialize(row, table);
  size_t offset = 0;
  uint64_t xmin = encoding::read_uint64(buffer, offset);
  uint64_t xmax = encoding::read_uint64(buffer, offset);
  uint16_t flags = static_cast<uint16_t>(encoding::read_uint16(buffer, offset));
  ASSERT_EQ(xmin, 0);
  ASSERT_EQ(xmax, 0);
  ASSERT_EQ(flags, 0x0000 | ROW_HASNULL);

  uint8_t nullbyte = encoding::read_uint8(buffer, offset);
  uint8_t expected_nulls = (1 << 2) | (1 << 3); // is_registered, course_code
  ASSERT_EQ(nullbyte, expected_nulls);

  int64_t id = static_cast<int64_t>(encoding::read_uint64(buffer, offset));
  ASSERT_EQ(id, 1234);

  uint32_t email_length = encoding::read_uint32(buffer, offset);
  ASSERT_EQ(email_length, sizeof("foo@bar.com") - 1);
  std::string email = encoding::read_str(buffer, email_length, offset);
  EXPECT_EQ(email, "foo@bar.com");

  uint32_t notes_length = encoding::read_uint32(buffer, offset);
  ASSERT_EQ(notes_length, sizeof("baz") - 1);
  std::string notes = encoding::read_str(buffer, notes_length, offset);
  EXPECT_EQ(notes, "baz");

  ASSERT_EQ(offset, buffer.size());
}

TEST_F(RecordManagerTest, DeserializeBasic) {
  std::vector<std::byte> buffer;

  schema::Table table;
  table.name = "test";
  table.tbl_name = "test";
  table.type = schema::SchemaType::TABLE;

  table.columns.push_back({
      .type = schema::DataType::TEXT,
      .name = "text",
      .is_primary_key = false,
      .is_nullable = false,
  });
  table.columns.push_back({
      .type = schema::DataType::INTEGER,
      .name = "id",
      .is_primary_key = false,
      .is_nullable = false,
  });
  table.columns.push_back({
      .type = schema::DataType::BOOLEAN,
      .name = "flag",
      .is_primary_key = false,
      .is_nullable = false,
  });

  uint64_t xmin, xmax = 0;
  uint16_t flags = 0x0000;

  encoding::append_uint64(buffer, xmin);
  encoding::append_uint64(buffer, xmax);
  encoding::append_uint16(buffer, flags);

  std::string text = "foobarbaz";
  encoding::append_uint32(buffer, static_cast<uint32_t>(text.size()));
  encoding::append_str(buffer, text);

  int64_t id = 12345;
  encoding::append_uint64(buffer, static_cast<uint64_t>(id));

  bool flag = true;
  encoding::append_bool(buffer, flag);

  RecordManager record;
  record::Row row = record.deserialize(buffer, table);

  ASSERT_EQ(row.values.size(), 3);
  ASSERT_TRUE(std::holds_alternative<record::Text>(row.values[0]));
  ASSERT_TRUE(std::holds_alternative<record::Integer>(row.values[1]));
  ASSERT_TRUE(std::holds_alternative<record::Bool>(row.values[2]));

  ASSERT_EQ(std::get<record::Text>(row.values[0]), "foobarbaz");
  ASSERT_EQ(std::get<record::Integer>(row.values[1]), 12345);
  ASSERT_EQ(std::get<record::Bool>(row.values[2]), true);
}

TEST_F(RecordManagerTest, DeserializeNulls) {
  std::vector<std::byte> buffer;

  schema::Table table;
  table.name = "test";
  table.tbl_name = "test";
  table.type = schema::SchemaType::TABLE;

  table.columns.push_back({
      .type = schema::DataType::TEXT,
      .name = "text",
      .is_primary_key = false,
      .is_nullable = false,
  });
  table.columns.push_back({
      .type = schema::DataType::INTEGER,
      .name = "id",
      .is_primary_key = false,
      .is_nullable = true,
  });
  table.columns.push_back({
      .type = schema::DataType::BOOLEAN,
      .name = "flag",
      .is_primary_key = false,
      .is_nullable = false,
  });

  uint64_t xmin, xmax = 0;
  uint16_t flags = 0x0000 | ROW_HASNULL;

  encoding::append_uint64(buffer, xmin);
  encoding::append_uint64(buffer, xmax);
  encoding::append_uint16(buffer, flags);

  uint8_t nullbyte = (1 << 1);
  encoding::append_uint8(buffer, nullbyte);

  std::string text = "foobarbaz";
  encoding::append_uint32(buffer, static_cast<uint32_t>(text.size()));
  encoding::append_str(buffer, text);

  // id is null, no bytes written

  bool flag = true;
  encoding::append_bool(buffer, flag);

  RecordManager record;
  record::Row row = record.deserialize(buffer, table);

  ASSERT_EQ(row.values.size(), 3);
  ASSERT_TRUE(std::holds_alternative<record::Text>(row.values[0]));
  ASSERT_TRUE(std::holds_alternative<record::Null>(row.values[1]));
  ASSERT_TRUE(std::holds_alternative<record::Bool>(row.values[2]));

  ASSERT_EQ(std::get<record::Text>(row.values[0]), "foobarbaz");
  ASSERT_EQ(std::get<record::Bool>(row.values[2]), true);
}
