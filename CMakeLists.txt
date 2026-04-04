cmake_minimum_required(VERSION 3.14)
project(cpplite)

include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/tests)

# NOTE(andrew): GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG 0.8.0
)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(yaml-cpp)

set(ANTLR4_INSTALL OFF CACHE BOOL "" FORCE)
set(WITH_DEMO OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(antlr4_runtime)

file(GLOB PARSER_SOURCES lib/parser/*.cpp)
add_library(parser STATIC ${PARSER_SOURCES})
target_include_directories(parser PUBLIC
  lib/parser
  ${antlr4_runtime_SOURCE_DIR}/runtime/Cpp/runtime/src
)
target_link_libraries(parser antlr4_static)

enable_testing()

file(GLOB VFS_TEST_SOURCES tests/vfs/*.cpp)
file(GLOB PAGER_TEST_SOURCES tests/pager/*.cpp)
file(GLOB_RECURSE BTREE_TEST_SOURCES tests/btree/*.cpp)

add_executable(
  vfs_tests
  tests/test_main.cpp
  ${VFS_TEST_SOURCES}
  tests/test_utils.h
  tests/test_utils.cpp
  lib/vfs/osfile.h
  lib/vfs/osfile.cpp
)

add_executable(
  pager_tests
  tests/test_main.cpp
  ${PAGER_TEST_SOURCES}
  tests/test_utils.h
  tests/test_utils.cpp
  tests/db_test_fixture.h
  lib/vfs/osfile.h
  lib/vfs/osfile.cpp
  lib/pager/pager.h
  lib/pager/pager.cpp
  lib/pager/first_page/first_page_manager.cpp
  lib/pager/free_page/free_page_manager.cpp
  lib/pager/node_page/node_page_manager.cpp
  lib/pager/leaf_page/leaf_page_manager.cpp
  lib/pager/overflow_page/overflow_page_manager.cpp
)

add_executable(
  btree_tests
  tests/test_main.cpp
  ${BTREE_TEST_SOURCES}
  tests/test_utils.h
  tests/test_utils.cpp
  tests/db_test_fixture.h
  tests/btree/btree_test_utils.h
  tests/btree/btree_test_utils.cpp
  tests/btree/builder/btree_builder.h
  tests/btree/builder/btree_builder.cpp
  tests/btree/builder/btree_validator.cpp
  lib/vfs/osfile.h
  lib/vfs/osfile.cpp
  lib/pager/pager.h
  lib/pager/pager.cpp
  lib/pager/first_page/first_page_manager.cpp
  lib/pager/free_page/free_page_manager.cpp
  lib/pager/node_page/node_page_manager.cpp
  lib/pager/leaf_page/leaf_page_manager.cpp
  lib/pager/overflow_page/overflow_page_manager.cpp
  lib/btree/btree.h
  lib/btree/btree.cpp
)

target_link_libraries(
  vfs_tests
  GTest::gtest
)

target_link_libraries(
  pager_tests
  GTest::gtest
)

target_link_libraries(
  btree_tests
  GTest::gtest
  yaml-cpp::yaml-cpp
)

target_include_directories(btree_tests PRIVATE
  tests/btree/btree_builder
)

add_custom_command(TARGET btree_tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/tests/btree/configs
    ${CMAKE_BINARY_DIR}/tests/btree/configs
)

include(GoogleTest)
gtest_discover_tests(vfs_tests)
gtest_discover_tests(pager_tests)
gtest_discover_tests(btree_tests)