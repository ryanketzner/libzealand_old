cmake_minimum_required(VERSION 3.16.3)

project(zorder)

# Build the Zealand library
add_library(Zealand Zealand.cpp)
add_executable(zz ztester.cpp)

include_directories(PUBLIC lib/libmorton/include/libmorton lib/GeometricTools/GTE)
target_link_libraries(zz Zealand)

# Include the test directory
add_subdirectory(test)
