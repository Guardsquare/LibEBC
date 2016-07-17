cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project (EmbeddedBC)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(LIBXML2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -O1 -g -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")
set(CMAKE_EXE_LINK_FLAGS_LIST "${CMAKE_EXE_LINK_FLAGS_LIST} -fsanitize=address")

if(COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINK_FLAGS_LIST "${CMAKE_EXE_LINK_FLAGS_LIST} -fprofile-arcs -ftest-coverage")
endif()

add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(tool)
