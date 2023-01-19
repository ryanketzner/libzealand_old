add_executable(ZealandTest ZealandTest.cpp)
add_executable(ZealandTest_Slow_0 ZealandTest_Slow_0.cpp)
add_executable(ZealandTest_Slow ZealandTest_Slow.cpp)
target_include_directories(ZealandTest PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(ZealandTest_Slow PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(ZealandTest_Slow_0 PUBLIC ${CMAKE_SOURCE_DIR})

# Link against Google Test
find_package(GTest REQUIRED)
target_link_libraries(ZealandTest GTest::GTest GTest::Main)
target_link_libraries(ZealandTest_Slow GTest::GTest GTest::Main)
target_link_libraries(ZealandTest_Slow_0 GTest::GTest GTest::Main)

# Link against the Zealand library
target_link_libraries(ZealandTest Zealand)
target_link_libraries(ZealandTest_Slow Zealand)
target_link_libraries(ZealandTest_Slow_0 Zealand)

# Add test to the build system
add_test(ZealandTest ZealandTest)
add_test(ZealandTest_Slow ZealandTest)
add_test(ZealandTest_Slow_0 ZealandTest)

