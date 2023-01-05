add_executable(ZealandTest ZealandTest.cpp)
target_include_directories(ZealandTest PUBLIC ${CMAKE_SOURCE_DIR})

# Link against Google Test
find_package(GTest REQUIRED)
target_link_libraries(ZealandTest GTest::GTest GTest::Main)

# Link against the Zealand library
target_link_libraries(ZealandTest Zealand)

# Add test to the build system
add_test(ZealandTest ZealandTest)

