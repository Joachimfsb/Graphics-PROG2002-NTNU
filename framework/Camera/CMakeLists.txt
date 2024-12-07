add_library(Camera PerspectiveCamera.cpp OrthographicCamera.cpp)
add_library(Framework::Camera ALIAS Camera)
target_include_directories(Camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Camera PUBLIC stb glm glad glfw)