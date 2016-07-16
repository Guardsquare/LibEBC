cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project (EmbeddedBC)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})

# Explicitly select components to link against
llvm_map_components_to_libnames(LLVM_LINK_LIBRARIES
  bitwriter
  core
  executionengine
  interpreter
  ipo
  irreader
  linker
  native
  support
  transformutils
  )

# LLVM compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-exceptions -fno-rtti -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")


find_package(LIBXML2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -O1 -g -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")
set(CMAKE_EXE_LINK_FLAGS_LIST "${CMAKE_EXE_LINK_FLAGS_LIST} -fsanitize=address")

add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(tool)
