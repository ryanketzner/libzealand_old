# Get all the .cpp files in the current directory
file(GLOB CPP_FILES *.cpp)

include_directories(${CMAKE_SOURCE_DIR})

find_package(GTest REQUIRED)
set(LIBS GTest::GTest GTest::Main fmt)

# Iterate through the .cpp files, creating executables and linking libraries
foreach(file ${CPP_FILES})
    get_filename_component(target_name ${file} NAME_WE)
    add_executable(${target_name} ${file})
    target_link_libraries(${target_name} ${LIBS})
    add_test(${target_name} ${target_name})
endforeach()
