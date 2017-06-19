set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE HEADER_FILES ${HEADER_DIR}/*.hpp)

add_library(rang INTERFACE)
target_include_directories(rang INTERFACE ${HEADER_DIR})
