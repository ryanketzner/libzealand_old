add_executable(ZCurves ZCurves.cpp)
add_executable(ZCurves3D ZCurves3D.cpp)
add_executable(Sphere Sphere.cpp)
add_executable(Cone Cone.cpp)
add_executable(SphereCone SphereCone.cpp)

include_directories(${CMAKE_SOURCE_DIR})
set(LIBS fmt)

target_link_libraries(ZCurves ${LIBS})
target_link_libraries(ZCurves3D ${LIBS})
target_link_libraries(Sphere ${LIBS})
target_link_libraries(Cone ${LIBS})
target_link_libraries(SphereCone ${LIBS})
