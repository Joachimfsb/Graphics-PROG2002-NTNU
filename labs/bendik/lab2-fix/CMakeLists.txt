cmake_minimum_required(VERSION 3.15)

project("lab #2 - fix")

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)

add_executable(lab2-fix main.cpp)

target_link_libraries(lab2-fix Framework glfw glad)
