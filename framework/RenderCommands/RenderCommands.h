#ifndef RENDERCOMMANDS_H
#define RENDERCOMMANDS_H

#include <string>
#include <string.h>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "VertexArray.h"

namespace Framework {

    namespace RenderCommands {
        inline void Clear(GLuint mode = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        {
            glClear(mode);
        }

        inline void SetPolygonMode(GLenum face, GLenum mode)
        {
            glPolygonMode(face, mode);
        }

        inline void DrawIndex(const std::shared_ptr<VertexArray>& vao, GLenum primitive)
        {
            glDrawElements(primitive, vao->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
        }

        inline void SetClearColor(glm::vec4 color)
        {
            glClearColor(color.x, color.y, color.z, color.w);  
        }

        inline void SetWireframeMode()
        {
            SetPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        inline void SetSolidMode()
        {
            SetPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    };
};

#endif