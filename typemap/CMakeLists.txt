cmake_minimum_required(VERSION 3.31)
project(typemap LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(typemap INTERFACE)

target_include_directories(typemap INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_compile_features(typemap INTERFACE
    cxx_std_20
)

target_compile_options(typemap INTERFACE
    -Wall -O3
)
