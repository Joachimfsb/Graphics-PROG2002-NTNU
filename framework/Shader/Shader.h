#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <string.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Framework {

  class Shader
  {
  public:
    Shader(const std::string &vertexSrc, const std::string &fragmentSrc);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    // Uniforms
    void UploadUniformMatrix4(const std::string& name, const glm::mat4& mat);
    void UploadUniformFloat(const std::string& name, const float f);
    void UploadUniformFloat2(const std::string& name, const glm::vec2& vector);
    void UploadUniformFloat3(const std::string& name, const glm::vec3& vector);
    void UploadUniformFloat4(const std::string& name, const glm::vec4& vector);
    void UploadUniformInt1(const std::string& name, const GLint x);
    void UploadUniformInt2(const std::string& name, const GLint x, const GLint y);
    void UploadUniformUInt1(const std::string& name, const GLuint x);

  private:
    GLuint ShaderProgram;  

    GLuint CompileShader(GLenum shaderType, const std::string &shaderSrc);
  };
};

#endif