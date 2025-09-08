cmake_minimum_required(VERSION 3.31)
project(staticmap_bundle LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(staticmap)
add_subdirectory(tests)
add_subdirectory(samples)
