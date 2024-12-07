#ifndef DESLOC_H
#define DESLOC_H

#include <glm/glm.hpp>
#include <memory>

#include "VertexArray.h"
#include "Shader.h"

#include "board.h"

class DesLoc {
  private:
    std::shared_ptr<Framework::VertexArray> vertexArray;
    std::shared_ptr<Framework::Shader> shader;
    glm::mat4 modelMatrix;
    glm::mat4 initModelMatrix;
    glm::vec4 color;
    Board::Pos pos;
    
  public:
    DesLoc(int x, int y, glm::vec4 color);
    ~DesLoc() { }   

    void SetPosition(Board::Pos pos);
    Board::Pos GetPosition() const { return pos; }

    void Draw(glm::mat4 cameraModel, std::shared_ptr<glm::vec4> overrideColor = nullptr);
};

#endif