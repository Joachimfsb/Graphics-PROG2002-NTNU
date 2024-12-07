cmake_minimum_required(VERSION 3.15)

project("exam23")

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)

add_executable(exam23 main.cpp board.cpp assignment.cpp piece.cpp desLoc.cpp)

target_link_libraries(exam23 Framework stb glm glfw glad)