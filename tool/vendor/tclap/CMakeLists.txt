set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE HEADER_FILES ${HEADER_DIR}/*.h)

add_library(tclap INTERFACE)
target_include_directories(tclap INTERFACE ${HEADER_DIR})
