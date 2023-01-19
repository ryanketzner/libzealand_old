cmake_minimum_required(VERSION 3.16.3)

project(zorder)

# Build the Zealand library
add_library(Zealand Zealand.cpp)

add_executable(zz ztester.cpp)

target_include_directories(zz PUBLIC lib/libmorton/include/libmorton)
target_include_directories(zz PUBLIC lib/GeometricTools/GTE)
target_link_libraries(zz Zealand)

target_include_directories(Zealand PUBLIC lib/libmorton/include/libmorton)
target_include_directories(Zealand PUBLIC lib/GeometricTools/GTE)

# Include the test directory
add_subdirectory(test)
