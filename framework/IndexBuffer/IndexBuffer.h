#ifndef INDEXBUFFER_H_
#define INDEXBUFFER_H_

#include <glad/glad.h>

namespace Framework {
  
  class IndexBuffer {
  public:
    // Constructor. Initializes the class with a data buffer and its size.
    // Note: The buffer will be bound upon construction, and the size is
    // specified in the number of elements, not bytes.
    IndexBuffer(GLuint *indices, GLsizei count);
    ~IndexBuffer();

    // Bind the vertex buffer.
    void Bind() const;

    // Unbind the vertex buffer.
    void Unbind() const;

    // Get the number of elements.
    inline GLuint GetCount() const { return Count; }

  private:
    GLuint IndexBufferID;
    GLuint Count;
  };
};

#endif