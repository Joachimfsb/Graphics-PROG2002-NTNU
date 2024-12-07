add_library(VertexArray VertexArray.cpp)
add_library(Framework::VertexArray ALIAS VertexArray)
target_include_directories(VertexArray PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(VertexArray PUBLIC IndexBuffer VertexBuffer glad glfw)
