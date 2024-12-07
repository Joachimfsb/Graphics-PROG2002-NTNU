cmake_minimum_required(VERSION 3.15)

project("lab #2")

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)

add_executable(lab2 main.cpp)

target_link_libraries(lab2 Framework glfw glad)
