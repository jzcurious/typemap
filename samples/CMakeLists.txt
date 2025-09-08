cmake_minimum_required(VERSION 3.31)
project(staticmap_samples LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(staticmap_samples ${SRC})

target_compile_features(staticmap_samples INTERFACE
    cxx_std_20
)

target_compile_options(staticmap_samples INTERFACE
    -Wall -O3
)

target_link_libraries(staticmap_samples PRIVATE
    staticmap
)
