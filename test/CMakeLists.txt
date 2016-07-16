enable_testing()
find_package(GTest REQUIRED)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}/*.cpp)

add_executable(ebctest ${SOURCE_FILES})
target_link_libraries(ebctest ebc ${GTEST_BOTH_LIBRARIES})
target_include_directories(ebctest PRIVATE ${GTEST_INCLUDE_DIRS})

add_test(LibEBCTests ebctest)

# Use C++14
set_property(TARGET ebctest PROPERTY CXX_STANDARD 14)
