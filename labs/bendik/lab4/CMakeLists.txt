cmake_minimum_required(VERSION 3.15)

project("lab4b")

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)

add_executable(lab4b main.cpp)

target_link_libraries(lab4b Framework stb glm glfw glad)


add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/cube_texture.jpeg
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/cube_texture.jpeg)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/floor_texture.jpeg
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/floor_texture.jpeg)


target_compile_definitions(${PROJECT_NAME} PRIVATE
  TEXTURES_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/")
