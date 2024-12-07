cmake_minimum_required(VERSION 3.15)

project("lab #2")

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)

add_executable(lab2-joa main.cpp)

target_link_libraries(lab2-joa Framework glfw glad)
