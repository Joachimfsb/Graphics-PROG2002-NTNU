#include <iostream>
#include <array>
#include "Shader.h"

namespace Framework {

    Shader::Shader(const std::string &vertexSrc, const std::string &fragmentSrc) {

        auto vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
        if (vertexShader == 0) {
            std::cout << "Failed to compile vertex shader! Exitting ...";
            std::exit(1);
        }
        auto fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);
        if (fragmentShader == 0) {
            std::cout << "Failed to compile fragment shader! Exitting ...\n";
            std::exit(1);
        }

        // Create a shader program
        ShaderProgram = glCreateProgram();
        glAttachShader(ShaderProgram, vertexShader);
        glAttachShader(ShaderProgram, fragmentShader);

        glLinkProgram(ShaderProgram);
        GLint result;
        glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &result);
        if (result == GL_FALSE) {
            std::cout << "Linking of shaders failed! Exitting...\n";
            std::exit(1);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    Shader::~Shader() {
        glDeleteProgram(ShaderProgram); //deletes the specified shader program
    }

    void Shader::Bind() const {
        glUseProgram(ShaderProgram);
    }

    void Shader::Unbind() const {
        glUseProgram(0);
    }

    void Shader::UploadUniformMatrix4(const std::string& name, const glm::mat4& mat) {
        glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void Shader::UploadUniformFloat(const std::string& name, const float f) {
        glUniform1f(glGetUniformLocation(ShaderProgram, name.c_str()), f);
    }
    void Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& vector) {
        glUniform2f(glGetUniformLocation(ShaderProgram, name.c_str()), vector.x, vector.y);
    }
    void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& vector) {
        glUniform3f(glGetUniformLocation(ShaderProgram, name.c_str()), vector.x, vector.y, vector.z);
    }
    void Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& vector) {
        glUniform4f(glGetUniformLocation(ShaderProgram, name.c_str()), vector.x, vector.y, vector.z, vector.w);
    }
    void Shader::UploadUniformInt1(const std::string& name, const GLint x) {
        glUniform1i(glGetUniformLocation(ShaderProgram, name.c_str()), x);
    }
    void Shader::UploadUniformInt2(const std::string& name, const GLint x, const GLint y) {
        glUniform2i(glGetUniformLocation(ShaderProgram, name.c_str()), x, y);
    }
    void Shader::UploadUniformUInt1(const std::string& name, const GLuint x) {
        glUniform1ui(glGetUniformLocation(ShaderProgram, name.c_str()), x);
    }

    GLuint Shader::CompileShader(GLenum shaderType, const std::string &shaderSrc) {

        GLuint shader = glCreateShader(shaderType);
        const GLchar* source = shaderSrc.c_str();
        glShaderSource(shader, 1, &source, nullptr);

        glCompileShader(shader);
        GLint result;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            // Print error log
            GLint length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length); // Get log length
            if (length > 0) {
                GLchar* log = new GLchar[length];
                glGetShaderInfoLog(shader, length, NULL, log); // Get the log

                std::cout << log; // Print the log

                delete log;
            }
            // Return
            return 0;
        }
        
        return shader;
    }
}