#include <glm/gtc/matrix_transform.hpp>

#include "GeometricTools.h"
#include "RenderCommands.h"

#include "desLoc.h"
#include "shaders/desLoc.glsh"
#include "board.h"

using namespace Framework;

DesLoc::DesLoc(int x, int y, glm::vec4 color) {
    pos = Board::Pos(x, y);
    this->color = color;

    // Create data structure for vertecies and color
    auto cubeVertices = GeometricTools::UnitCubeGeometry3D;
    auto cubeIndices = GeometricTools::UnitCubeTopology3D;

    // Vertex Buffer
    auto vb = std::make_shared<VertexBuffer>(cubeVertices.data(), cubeVertices.size() * sizeof(cubeVertices[0])); // Vertex Buffer Object
    BufferLayout vblayout = {
        {ShaderDataType::Float3, "a_Position"}
    };
    vb->SetLayout(vblayout);

    // Index buffer
    auto ib = std::make_shared<IndexBuffer>(cubeIndices.data(), cubeIndices.size()); // Index Buffer Object
    
    // Vertex Array
    vertexArray = std::make_shared<VertexArray>(); // Vertex Array Object
    vertexArray->AddVertexBuffer(vb);
    vertexArray->SetIndexBuffer(ib);

    // Shader
    shader = std::make_shared<Shader>(P_VERTEX_SHADER, P_FRAGMENT_SHADER);

    // Init model matrix
    float xoffset = BOARD_SQUARE_XSIZE/2.0f;
    float yoffset = BOARD_SQUARE_YSIZE/2.0f;
    initModelMatrix = 
          glm::translate(glm::mat4(1.0f), {1.0f - xoffset, 1.0f - yoffset, 0.01f})
        * glm::scale(glm::mat4(1.0f), {0.1, 0.1, 0.1}); 

    // Set position
    SetPosition(pos);
}

void DesLoc::SetPosition(Board::Pos pos) {
    // Set position
    this->pos = pos;

    // Update model matrix
    modelMatrix = 
          glm::translate(glm::mat4(1.0f), {-pos.x * BOARD_SQUARE_XSIZE, -pos.y * BOARD_SQUARE_YSIZE, 0.0f})
        * initModelMatrix;
}



void DesLoc::Draw(glm::mat4 cameraModel, std::shared_ptr<glm::vec4> overrideColor) {

    vertexArray->Bind();          
    shader->Bind();

    shader->UploadUniformMatrix4("u_Model", modelMatrix);
    shader->UploadUniformMatrix4("u_ViewProjection", cameraModel);
    if (overrideColor)
        shader->UploadUniformFloat4("u_Color", *overrideColor);
    else
        shader->UploadUniformFloat4("u_Color", color);

    RenderCommands::DrawIndex(vertexArray, GL_TRIANGLES);
}