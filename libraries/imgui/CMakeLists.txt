cmake_minimum_required(VERSION 3.0)
project(imgui)

file(GLOB headers "*.h")
file(GLOB sources "*.cpp")

add_library(imgui ${headers} ${sources})
target_include_directories(imgui PUBLIC ${headers})
if(MINGW)
    find_package(glfw3 3.3 REQUIRED)
    target_link_libraries(imgui glfw)
endif()