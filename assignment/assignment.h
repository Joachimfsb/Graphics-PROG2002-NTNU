#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

// include area
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

#include "GLFWApplication.h"
#include "PerspectiveCamera.h"

#include "board.h"
#include "piece.h"

// GLFW Key callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


class Assignment : public Framework::GLFWApplication {
private:
    std::shared_ptr<Board> board;
    std::vector<Piece> pieces; // Due to the small number of pieces, a simple vector of objects is
                               //  sufficient for our purpose.

    // Camera
    std::shared_ptr<Framework::PerspectiveCamera> camera;
    float cameraRotation = 0;
    const float cameraDistance = 3;

    // Config
    Board::Pos markedSquare = Board::Pos(0,0);
    Piece* selectedPiece = nullptr;

    // Runtime
    double time = 0.0f; // Total time since start
    double dtime = 0.0f; // Time since last time update

    // Functions //
    void parseInput();

    void rotateCamera(int deltaDegrees);
    void zoomCamera(int deltaDegrees);
    void moveMarkedSquare(int deltaX, int deltaY);
    void selectPiece();
    void movePiece();

    Piece* getPieceAtPos(Board::Pos p);
public:
    // Input
    std::vector<int> keysDown; // Keys that are currently pressed down

    // Constructor / Destructor
    Assignment(const std::string& name,  const std::string& version, const int screenWidth, const int screenHeight) : GLFWApplication(name, version, screenWidth, screenHeight) {}
    ~Assignment() {
        glfwDestroyWindow(window);
    }

    // Virtual
    virtual bool Init();
    virtual void Run();
};

extern Assignment assignment;

#endif