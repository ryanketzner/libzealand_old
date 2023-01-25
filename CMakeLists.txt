cmake_minimum_required(VERSION 3.16.3)

project(zorder)

include_directories(PUBLIC lib/libmorton/include/libmorton lib/GeometricTools/GTE)

# Include the test directory
add_subdirectory(test)
