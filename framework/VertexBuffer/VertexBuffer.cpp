#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "VertexBuffer.h"

namespace Framework {

    VertexBuffer::VertexBuffer(const void *vertices, GLsizei size) {

        // Generate VBO
        glGenBuffers(1, &VertexBufferID); // Create VBO

        // Send data to VBO (GPU)
        Bind();
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW); 
        Unbind();
    }

    VertexBuffer::~VertexBuffer() {
        glDeleteBuffers(1, &VertexBufferID);
    }

    void VertexBuffer::Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID); // Bind it
    }

    void VertexBuffer::Unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind it
    }

    void VertexBuffer::BufferSubData(GLintptr offset, GLsizeiptr size, const void *data) const {
        Bind();
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        Unbind();
    }
};