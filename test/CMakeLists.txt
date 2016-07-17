add_subdirectory(catch)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}/*.cpp)

add_executable(ebctest ${SOURCE_FILES})
add_dependencies(ebctest catch)
target_link_libraries(ebctest ebc)
target_include_directories(ebctest PRIVATE ${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})

enable_testing(true)
add_test(ebctest ebctest)

# Use C++14
set_property(TARGET ebctest PROPERTY CXX_STANDARD 14)
