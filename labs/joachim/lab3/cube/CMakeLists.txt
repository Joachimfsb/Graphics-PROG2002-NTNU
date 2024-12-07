cmake_minimum_required(VERSION 3.15)

project("cube")
set(CMAKE_BUILD_TYPE Debug)
find_package(OpenGL REQUIRED)
add_executable(lab3-cube-joa cube.cpp)
target_link_libraries(lab3-cube-joa Framework glm glfw glad)
