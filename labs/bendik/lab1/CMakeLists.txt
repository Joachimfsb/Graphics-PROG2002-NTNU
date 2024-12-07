cmake_minimum_required(VERSION 3.15)

project("lab #1")

find_package(OpenGL REQUIRED)

add_executable(lab1 main.cpp)

target_link_libraries(lab1 glfw glad)
