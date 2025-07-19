cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB VFS_TEST_SOURCES tests/vfs/*.cpp)
file(GLOB PAGER_TEST_SOURCES tests/pager/*.cpp)

add_executable(
  vfs_tests
  tests/test_main.cpp
  ${VFS_TEST_SOURCES}
  lib/vfs/osfile.h
  lib/vfs/osfile.cpp
)
add_executable(
  pager_tests
  tests/test_main.cpp
  ${PAGER_TEST_SOURCES}
  lib/vfs/osfile.h
  lib/vfs/osfile.cpp
  lib/pager/pager.h
  lib/pager/pager.cpp
  lib/pager/page_manager.cpp
)

target_link_libraries(
  vfs_tests
  GTest::gtest
)

target_link_libraries(
  pager_tests
  GTest::gtest
)

include(GoogleTest)
gtest_discover_tests(vfs_tests)
gtest_discover_tests(pager_tests)