cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project (LibEBC)

# Version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

# Export compile database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Static libs
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBRARIES OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -g -O0 -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -O0 -g -fno-omit-frame-pointer ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")

add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(tool)
