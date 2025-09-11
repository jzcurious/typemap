cmake_minimum_required(VERSION 3.31)
project(staticmap LANGUAGES CXX)

add_library(staticmap INTERFACE)

target_include_directories(staticmap INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_compile_features(staticmap INTERFACE
    cxx_std_20
)

target_compile_options(staticmap INTERFACE
    -Wall -O3
)
