include(CheckLibraryExists)

# LibXml2 Dependency
find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

# LLVM Dependency
find_package(LLVM REQUIRED CONFIG HINTS ${LLVM_HINT})
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
include_directories(${LLVM_INCLUDE_DIRS})

# Explicitly select components to link against
llvm_map_components_to_libnames(LLVM_LINK_LIBRARIES
  core
  object
  support
  )

# LLVM compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-exceptions -fno-rtti -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")

# Xar
check_library_exists(xar xar_open "" HAVE_LIBXAR)
if(HAVE_LIBXAR)
  set(XAR_LIB xar)
endif()

# Configure header
configure_file (
  "${PROJECT_SOURCE_DIR}/lib/include/ebc/Config.h.in"
  "${PROJECT_BINARY_DIR}/ebc/Config.h")

# LibEBC
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}/*.cpp)

add_library(ebc ${SOURCE_FILES})
target_link_libraries(ebc ${XAR_LIB} ${LLVM_LINK_LIBRARIES} ${LIBXML2_LIBRARIES})
target_include_directories(ebc PRIVATE  ${PROJECT_BINARY_DIR})
target_include_directories(ebc PUBLIC ${HEADER_DIR})

# Use C++14
set_property(TARGET ebc PROPERTY CXX_STANDARD 14)
