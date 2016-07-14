set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}/*.cpp)

add_library(ebc ${SOURCE_FILES})
target_link_libraries(ebc xar ${LLVM_LINK_LIBRARIES} ${LIBXML2_LIBRARIES})
target_include_directories(ebc PUBLIC ${HEADER_DIR})

# Use C++14
set_property(TARGET ebc PROPERTY CXX_STANDARD 14)
