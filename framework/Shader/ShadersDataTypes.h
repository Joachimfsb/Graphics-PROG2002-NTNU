#ifndef SHADERSDATATYPES_H_
#define SHADERSDATATYPES_H_

#include <glad/glad.h>

namespace Framework {

  // =============================================================================
  // ShaderDataType enum
  // =============================================================================
  enum class ShaderDataType
  {
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool
  };

  // =============================================================================
  // ShaderDataTypeSize
  // =============================================================================
  constexpr GLsizei ShaderDataTypeSize(ShaderDataType type)
  {
    switch (type)
    {
      case ShaderDataType::Float: return 4;
      case ShaderDataType::Float2: return 4 * 2;
      case ShaderDataType::Float3: return 4 * 3;
      case ShaderDataType::Float4: return 4 * 4;
      case ShaderDataType::Mat3: return 4 * 3 * 3;
      case ShaderDataType::Mat4: return 4 * 4 * 4;
      case ShaderDataType::Int: return 4;
      case ShaderDataType::Int2: return 4 * 2;
      case ShaderDataType::Int3: return 4 * 3;
      case ShaderDataType::Int4: return 4 * 4;
      case ShaderDataType::Bool: return 1;
      case ShaderDataType::None: return 0;
    }

    return 0;
  }

  constexpr GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
  {
    switch (type)
    {
      case ShaderDataType::Float: return GL_FLOAT;
      case ShaderDataType::Float2: return GL_FLOAT;
      case ShaderDataType::Float3: return GL_FLOAT;
      case ShaderDataType::Float4: return GL_FLOAT;
      case ShaderDataType::Mat3: return GL_FLOAT;
      case ShaderDataType::Mat4: return GL_FLOAT;
      case ShaderDataType::Int: return GL_INT;
      case ShaderDataType::Int2: return GL_INT;
      case ShaderDataType::Int3: return GL_INT;
      case ShaderDataType::Int4: return GL_INT;
      case ShaderDataType::Bool: return GL_INT;
      case ShaderDataType::None: return GL_INT;
    }

    return GL_INT;
  }

  constexpr GLsizei ShaderDataTypeComponentCount(ShaderDataType type)
  {
    switch (type)
    {
      case ShaderDataType::Float: return 1;
      case ShaderDataType::Float2: return 2;
      case ShaderDataType::Float3: return 3;
      case ShaderDataType::Float4: return 4;
      case ShaderDataType::Mat3: return 3 * 3;
      case ShaderDataType::Mat4: return 4 * 4;
      case ShaderDataType::Int: return 1;
      case ShaderDataType::Int2: return 2;
      case ShaderDataType::Int3: return 3;
      case ShaderDataType::Int4: return 4;
      case ShaderDataType::Bool: return 1;
      case ShaderDataType::None: return 0;
    }
    
    return 0;
  }
};

#endif // SHADERSDATATYPES_H_
