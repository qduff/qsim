cmake_minimum_required(VERSION 3.24)
project(test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_subdirectory(glad/)

add_executable(test main.cpp utils/loadmcm.cpp kernelinterface.cpp)



find_package(GLEW REQUIRED)
target_link_libraries(test GLEW::GLEW)

find_package(OpenGL REQUIRED)
target_link_libraries(test OpenGL::GL)

target_link_libraries(test /usr/lib64/libglfw.so)

target_link_libraries(test glad)
