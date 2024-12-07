cmake_minimum_required(VERSION 3.15)

project("assignment")

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)

add_executable(assignment main.cpp board.cpp assignment.cpp piece.cpp)

target_link_libraries(assignment Framework stb glm glfw glad)