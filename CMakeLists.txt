cmake_minimum_required(VERSION 3.31)
project(typemap_bundle LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(typemap)
add_subdirectory(tests)
