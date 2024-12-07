cmake_minimum_required(VERSION 3.15)

project("cube13")
set(CMAKE_BUILD_TYPE Debug)
find_package(OpenGL REQUIRED)
add_executable(issue13-joa-cube cube.cpp)
target_link_libraries(issue13-joa-cube Framework glm glfw glad)
