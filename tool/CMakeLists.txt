add_subdirectory(vendor)

# Configure
configure_file (
  "${PROJECT_SOURCE_DIR}/tool/ebcutil.cpp"
  "${PROJECT_BINARY_DIR}/tool/ebcutil.cpp")

add_executable(ebcutil ${PROJECT_BINARY_DIR}/tool/ebcutil.cpp)
target_link_libraries(ebcutil ebc tclap rang)

set_property(TARGET ebcutil PROPERTY CXX_STANDARD 14)
