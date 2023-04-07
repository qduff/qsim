cmake_minimum_required(VERSION 3.24)
project(test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_subdirectory(libraries/glad/)
add_subdirectory(libraries/imgui/)


add_executable(test main.cpp utils/loadmcm.cpp kernelinterface.cpp overlay/overlay.cpp preferences.cpp)



find_package(GLEW REQUIRED)
target_link_libraries(test GLEW::GLEW)

find_package(OpenGL REQUIRED)
target_link_libraries(test OpenGL::GL)


# target_link_libraries(test /usr/lib64/libglfw.so)
# target_link_libraries(test /usr/x86_64-w64-mingw32/sys-root/mingw/lib/glfw3.dll)
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(test glfw)
target_link_libraries(test imgui)

target_link_libraries(test glad)
