cmake_minimum_required(VERSION 3.31)
project(staticmap_tests LANGUAGES CXX)

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

add_executable(staticmap_tests ${SRC})

target_compile_features(staticmap_tests INTERFACE
    cxx_std_20
)

target_compile_options(staticmap_tests INTERFACE
    -Wall -O3
)

target_link_libraries(staticmap_tests PRIVATE
    staticmap
    GTest::gtest_main
)

enable_testing()
gtest_discover_tests(staticmap_tests)
