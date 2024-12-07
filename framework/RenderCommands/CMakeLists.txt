add_library(RenderCommands INTERFACE)
add_library(Framework::RenderCommands ALIAS RenderCommands)
target_include_directories(RenderCommands INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})