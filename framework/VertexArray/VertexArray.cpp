#include "VertexArray.h"

namespace Framework {

    VertexArray::VertexArray() {
        // Create VAO
        glGenVertexArrays(1, &VertexArrayID); // Create VAO
    }

    VertexArray::~VertexArray() {
        glDeleteVertexArrays(1, &VertexArrayID);
    }

    void VertexArray::Bind() const {
        glBindVertexArray(VertexArrayID); // Bind it
    }

    void VertexArray::Unbind() const {
        glBindVertexArray(0); // Unbind it
    }

    void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) {

        Bind();
        vertexBuffer->Bind();

        // Get layout
        auto layout = vertexBuffer->GetLayout();

        int attrIndex = 0;
        for (auto attr : layout.GetAttributes()) {
            // Add a vertex attrib pointer
            // (This opengl call is what associates the bounded VAO with the bounded VBO)
            glVertexAttribPointer(attrIndex,
                                attr.Count,
                                ShaderDataTypeToOpenGLBaseType(attr.Type),
                                attr.Normalized,
                                layout.GetStride(),
                                (const void*)(intptr_t)attr.Offset);
            glEnableVertexAttribArray(attrIndex);
            attrIndex++;
        }

        // Add to data structure
        VertexBuffers.push_back(vertexBuffer);

        Unbind();
        vertexBuffer->Unbind();
    }

    void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer) {
        Bind();
        indexBuffer->Bind(); // This bind associates the VAO with the EBO in opengl
        IdxBuffer = indexBuffer;
        Unbind(); // Unbinding has to be in this order
        indexBuffer->Unbind();
    }
};