cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project (LibEBC)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ferror-limit=1 -g -O0 -g -fno-omit-frame-pointer ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")

add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(tool)
