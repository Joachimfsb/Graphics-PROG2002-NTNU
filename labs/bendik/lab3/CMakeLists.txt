cmake_minimum_required(VERSION 3.15)

project("lab #3")

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)

add_executable(lab3b main.cpp)

target_link_libraries(lab3b Framework glm glfw glad)
