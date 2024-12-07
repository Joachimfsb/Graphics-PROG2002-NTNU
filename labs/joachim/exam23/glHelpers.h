#ifndef GLHELPERS_H
#define GLHELPERS_H

// Only define if in debug mode
#ifndef NDEBUG

#include <GLFW/glfw3.h>


// Debug error callback
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {

    std::cerr << "GL CALLBACK:" << ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" )
              << "type = 0x" << type
              << ", severity = 0x" << severity
              << ", message =" << message << "\n";
}

#endif

#endif