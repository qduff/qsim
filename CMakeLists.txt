cmake_minimum_required(VERSION 3.24)
project(qsim)

if(MINGW OR WIN32)
add_definitions(-D_WIN32)
endif()

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


set(CMAKE_CXX_STANDARD 17)
# add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -fno-omit-frame-pointer ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fno-omit-frame-pointer ")



add_subdirectory(libraries/glad/)
add_subdirectory(libraries/tracy/)
add_subdirectory(libraries/imgui/)

add_definitions(-DTRACY_ENABLE)

add_executable(qsim main.cpp utils/loadmcm.cpp kernelinterface.cpp overlay/overlay.cpp preferences.cpp renderers/osd.cpp  logger/logger.cpp
renderers/scene.cpp input/input.cpp
)

if(MINGW OR WIN32)
message("mingw")
set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")
set(CMAKE_EXE_LINKER_FLAGS "--static ${CMAKE_EXE_LINKER_FLAGS}")
endif()


find_package(GLEW REQUIRED)
target_link_libraries(qsim GLEW::GLEW)

find_package(OpenGL REQUIRED)
target_link_libraries(qsim OpenGL::GL)

find_package(glfw3 3.3 REQUIRED)

# target_link_libraries(qsim /usr/lib64/libglfw.so)

target_link_libraries(qsim glfw)

target_link_libraries(qsim imgui)

target_link_libraries(qsim tracy)

target_link_libraries(qsim glad)
