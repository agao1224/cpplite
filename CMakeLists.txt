cmake_minimum_required(VERSION 3.14)
project(cpplite)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/tests)

# ── Third-party dependencies ──────────────────────────────────────────────────

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# NOTE(andrew): For Windows - prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  antlr4_runtime
  GIT_REPOSITORY https://github.com/antlr/antlr4.git
  GIT_TAG 4.13.2
  SOURCE_SUBDIR runtime/Cpp
)
set(ANTLR4_INSTALL OFF CACHE BOOL "" FORCE)
set(WITH_DEMO OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(antlr4_runtime)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 0.8.0
)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(yaml-cpp)

# ── Library targets ───────────────────────────────────────────────────────────

add_library(parser STATIC
  lib/parser/SQLiteLexer.cpp
  lib/parser/SQLiteParser.cpp
  lib/parser/SQLiteParserListener.cpp
  lib/parser/SQLiteParserBaseListener.cpp
)
target_include_directories(parser PUBLIC
  lib/parser
  ${antlr4_runtime_SOURCE_DIR}/runtime/Cpp/runtime/src
)
target_link_libraries(parser PUBLIC antlr4_static)

add_library(vfs STATIC
  lib/vfs/osfile.cpp
)
target_include_directories(vfs PUBLIC lib/vfs)

add_library(pager STATIC
  lib/pager/pager.cpp
  lib/pager/first_page/first_page_manager.cpp
  lib/pager/free_page/free_page_manager.cpp
  lib/pager/node_page/node_page_manager.cpp
  lib/pager/leaf_page/leaf_page_manager.cpp
  lib/pager/overflow_page/overflow_page_manager.cpp
)
target_include_directories(pager PUBLIC lib/pager)
target_link_libraries(pager PUBLIC vfs)

add_library(btree STATIC
  lib/btree/btree.cpp
  lib/btree/utils.cpp
)
target_include_directories(btree PUBLIC lib/btree)
target_link_libraries(btree PUBLIC pager)

add_library(catalog STATIC
  lib/catalog/catalog.cpp
  lib/catalog/serialize.cpp
)
target_include_directories(catalog PUBLIC lib/catalog)
target_link_libraries(catalog PUBLIC btree)

# ── Shared test utilities ─────────────────────────────────────────────────────

add_library(test_utils STATIC
  tests/test_utils.cpp
)
target_include_directories(test_utils PUBLIC tests)
target_link_libraries(test_utils PUBLIC GTest::gtest)

# ── Test executables ──────────────────────────────────────────────────────────

enable_testing()

file(GLOB VFS_TEST_SOURCES     tests/vfs/*.cpp)
file(GLOB PAGER_TEST_SOURCES   tests/pager/*.cpp)
file(GLOB_RECURSE BTREE_TEST_SOURCES  tests/btree/*.cpp)
file(GLOB CATALOG_TEST_SOURCES tests/catalog/*.cpp)

add_executable(vfs_tests
  tests/test_main.cpp
  ${VFS_TEST_SOURCES}
)
target_link_libraries(vfs_tests PRIVATE test_utils vfs GTest::gtest)

add_executable(pager_tests
  tests/test_main.cpp
  ${PAGER_TEST_SOURCES}
)
target_link_libraries(pager_tests PRIVATE test_utils pager GTest::gtest)

add_executable(btree_tests
  tests/test_main.cpp
  ${BTREE_TEST_SOURCES}
  tests/btree/builder/btree_builder.cpp
  tests/btree/builder/btree_validator.cpp
  tests/btree/btree_test_utils.cpp
)
target_include_directories(btree_tests PRIVATE tests/btree/builder)
target_link_libraries(btree_tests PRIVATE test_utils btree GTest::gtest yaml-cpp::yaml-cpp)
add_custom_command(TARGET btree_tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/tests/btree/configs
    ${CMAKE_BINARY_DIR}/tests/btree/configs
)

add_executable(catalog_tests
  tests/test_main.cpp
  ${CATALOG_TEST_SOURCES}
)
target_link_libraries(catalog_tests PRIVATE test_utils catalog GTest::gtest)
target_compile_definitions(catalog_tests PRIVATE CPPLITE_TESTING)

# ── Test discovery ────────────────────────────────────────────────────────────

include(GoogleTest)
gtest_discover_tests(vfs_tests)
gtest_discover_tests(pager_tests)
gtest_discover_tests(btree_tests)
gtest_discover_tests(catalog_tests)
