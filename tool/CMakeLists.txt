add_executable(ebcutil ebcutil.cpp)
target_link_libraries(ebcutil ebc)

set_property(TARGET ebcutil PROPERTY CXX_STANDARD 14)
