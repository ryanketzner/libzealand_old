add_executable(ZealandTest ZealandTest.cpp)
add_executable(ZealandTest_Slow_0 ZealandTest_Slow_0.cpp)
add_executable(ZealandTest_Slow ZealandTest_Slow.cpp)

include_directories(${CMAKE_SOURCE_DIR})

find_package(GTest REQUIRED)
set(LIBS GTest::GTest GTest::Main Zealand)

target_link_libraries(ZealandTest ${LIBS})
target_link_libraries(ZealandTest_Slow ${LIBS})
target_link_libraries(ZealandTest_Slow_0 ${LIBS})

# Add test to the build system
add_test(ZealandTest ZealandTest)
add_test(ZealandTest_Slow ZealandTest)
add_test(ZealandTest_Slow_0 ZealandTest)
