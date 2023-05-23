include_directories(${CMAKE_SOURCE_DIR})

#find_package(TBB REQUIRED)

#include_directories(${TBB_INCLUDE_DIRS})

add_executable(ZealandProf ZealandProf.cpp)
add_executable(ZealandProf_Par ZealandProf_Par.cpp)
add_executable(ZealandProf_Seq ZealandProf_Seq.cpp)

find_package(TBB)
if (TBB_FOUND)
	target_link_libraries(ZealandProf_Par -ltbb)
else()
	message("Parallel build not possible. Install tbb for parallel build.")
endif()
