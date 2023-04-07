cmake_minimum_required(VERSION 3.0)
project(imgui)

file(GLOB_RECURSE headers "*.h")
file(GLOB_RECURSE sources "*.cpp")

add_library(imgui ${headers} ${sources})
target_include_directories(imgui PUBLIC ${headers})
