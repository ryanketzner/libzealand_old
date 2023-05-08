add_executable(Sphere_bench Sphere.cpp)
add_executable(Cone_bench Cone.cpp)
add_executable(SphereCone_bench SphereCone.cpp)

include_directories(${CMAKE_SOURCE_DIR})
set(LIBS fmt)

target_link_libraries(Sphere_bench ${LIBS})
target_link_libraries(Cone_bench ${LIBS})
target_link_libraries(SphereCone_bench ${LIBS})
