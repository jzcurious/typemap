cmake_minimum_required(VERSION 3.31)
project(typemap_tests LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
include(GoogleTest)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(typemap_tests ${SRC})

target_compile_features(typemap_tests INTERFACE
    cxx_std_20
)

target_compile_options(typemap_tests INTERFACE
    -Wall -O3
)

target_link_libraries(typemap_tests PRIVATE
    typemap
    GTest::gtest_main
)

enable_testing()
gtest_discover_tests(typemap_tests)
