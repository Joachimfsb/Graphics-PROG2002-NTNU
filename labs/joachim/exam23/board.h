#ifndef BOARD_H
#define BOARD_H

#include <glm/glm.hpp>
#include <memory>
#include "VertexArray.h"
#include "Shader.h"
#include "TextureManager.h"

const int BOARD_ROWS = 10;
const int BOARD_COLS = 10;
constexpr float BOARD_SQUARE_XSIZE = 2.0f/(float)BOARD_COLS;
constexpr float BOARD_SQUARE_YSIZE = 2.0f/(float)BOARD_ROWS;

class Board {
  public:
    struct Pos {
        int x, y;
        Pos() { x = 0; y = 0; }
        Pos(int x, int y) : x(x), y(y) {}
        bool operator==(const Pos& r) { return x == r.x && y == r.y; }
    };

  private:
    std::shared_ptr<Framework::VertexArray> vertexArray;
    std::shared_ptr<Framework::Shader> shader;
    glm::mat4 modelMatrix;

  public:
    Board();
    ~Board() {}

    void Draw(glm::mat4 cameraModel, Pos markedSquare);
};



#endif