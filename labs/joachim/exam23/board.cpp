#include <glm/gtc/matrix_transform.hpp>

#include "RenderCommands.h"
#include "GeometricTools.h"

#include "board.h"
#include "shaders/chessboard.glsh"

using namespace Framework;

Board::Board() {

    // Initialize board
    auto chessBoardVertices = GeometricTools::UnitGridGeometry2D<BOARD_ROWS, BOARD_COLS>(); //data for verticies
    auto chessBoardIndices = GeometricTools::UnitGridTopologyTriangles<BOARD_ROWS, BOARD_COLS>(); //data for squares/incencies

    auto vb = std::make_shared<VertexBuffer>(chessBoardVertices.data(), chessBoardVertices.size() * sizeof(chessBoardVertices[0]));
    BufferLayout vboLayout = {
        {ShaderDataType::Float2, "v_Position"}
    };
    vb->SetLayout(vboLayout);
    
    auto ib = std::make_shared<IndexBuffer>(chessBoardIndices.data(), chessBoardIndices.size());

    vertexArray = std::make_shared<VertexArray>();
    vertexArray->AddVertexBuffer(vb);
    vertexArray->SetIndexBuffer(ib);

    // Shader
    shader = std::make_shared<Shader>(CB_VERTEX_SHADER, CB_FRAGMENT_SHADER);

    // Model
    modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), {0.0f, 0.0f, 1.0f});
}



void Board::Draw(glm::mat4 cameraModel, Pos markedSquare) {

    vertexArray->Bind();          
    shader->Bind();

    shader->UploadUniformMatrix4("u_Model", modelMatrix);
    shader->UploadUniformMatrix4("u_ViewProjection", cameraModel);
    shader->UploadUniformInt2("u_markedSquare", markedSquare.x, markedSquare.y);
    shader->UploadUniformInt2("u_gridLayout", BOARD_COLS, BOARD_ROWS);
    RenderCommands::DrawIndex(vertexArray, GL_TRIANGLES);
}