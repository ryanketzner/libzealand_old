cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")

project(zorder)

# Get the absolute path to the project root directory
get_filename_component(PROJECT_ROOT_DIR "${CMAKE_SOURCE_DIR}" ABSOLUTE)

# Pass the project root directory as a preprocessor definition
add_definitions(-DPROJECT_ROOT_DIR="${PROJECT_ROOT_DIR}")
add_definitions(-DPOJECT_BUILD_DIR="${PROJECT_ROOT_DIR}/build")


include_directories(PUBLIC lib/libmorton/include/libmorton lib/GeometricTools/GTE)

# Include the test directory
add_subdirectory(test)
add_subdirectory(profile)
