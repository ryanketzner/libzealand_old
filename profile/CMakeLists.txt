include_directories(${CMAKE_SOURCE_DIR})

#find_package(TBB REQUIRED)

#include_directories(${TBB_INCLUDE_DIRS})

find_package(TBB REQUIRED tbb)

add_executable(ZealandProf ZealandProf.cpp)
add_executable(ZealandProf_Par ZealandProf_Par.cpp)
add_executable(ZealandProf_Seq ZealandProf_Seq.cpp)


#target_link_libraries(ZealandProf_Par ${TBB_LIBRARIES})
target_link_libraries(ZealandProf_Par -ltbb)
