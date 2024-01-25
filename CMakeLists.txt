cmake_minimum_required(VERSION 3.14)

project(SimpleShell VERSION 1.0 DESCRIPTION "SimpleShell" LANGUAGES C CXX)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP false
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(shell_test gtest/shell_test.cpp)
target_link_libraries(shell_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(shell_test)

include_directories(include)
include_directories(src)
file(GLOB SOURCES_C "src/shell.c")
file(GLOB SOURCES_H "include/msgs.h")

add_executable(shell ${SOURCES_C} ${SOURCES_H})
