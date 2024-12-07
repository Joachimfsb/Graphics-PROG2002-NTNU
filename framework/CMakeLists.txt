# Wrapper library
add_library(Framework Framework.cpp)
target_include_directories(Framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Framework Camera TextureManager RenderCommands VertexArray Shader VertexBuffer IndexBuffer GeometricTools GLFWApplication)


# Sub directories
add_subdirectory(GLFWApplication)
add_subdirectory(GeometricTools)
add_subdirectory(VertexBuffer)
add_subdirectory(RenderCommands)
add_subdirectory(IndexBuffer)
add_subdirectory(VertexArray)
add_subdirectory(TextureManager)
add_subdirectory(Shader)
add_subdirectory(Camera)