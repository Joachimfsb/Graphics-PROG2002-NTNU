add_library(IndexBuffer IndexBuffer.cpp)
add_library(Framework::IndexBuffer ALIAS IndexBuffer)
target_include_directories(IndexBuffer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(IndexBuffer PUBLIC glad glfw)
