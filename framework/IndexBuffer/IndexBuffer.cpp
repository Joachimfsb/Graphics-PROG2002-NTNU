#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "IndexBuffer.h"

namespace Framework {
    IndexBuffer::IndexBuffer(GLuint *indices, GLsizei count) {

        Count = count; // Store count

        glGenBuffers(1, &IndexBufferID); // Create EBO

        // Send data to EBO (GPU)
        Bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, Count * sizeof(GLuint), indices, GL_STATIC_DRAW);
        Unbind();
    }

    IndexBuffer::~IndexBuffer() {
        glDeleteBuffers(1, &IndexBufferID); // Delete buffer
    }

    void IndexBuffer::Bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferID); // Bind it
    }

    void IndexBuffer::Unbind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind it
    }
};