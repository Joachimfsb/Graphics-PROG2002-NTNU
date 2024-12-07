add_library(VertexBuffer VertexBuffer.cpp)
add_library(Framework::VertexBuffer ALIAS VertexBuffer)
target_include_directories(VertexBuffer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VertexBuffer PUBLIC Shader glad glfw)
