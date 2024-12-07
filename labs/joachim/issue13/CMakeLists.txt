cmake_minimum_required(VERSION 3.15)

project("issue13")
set(CMAKE_BUILD_TYPE Debug)
find_package(OpenGL REQUIRED)
add_executable(issue13-joa main.cpp)
target_link_libraries(issue13-joa Framework glm glfw glad)
add_subdirectory(cube)