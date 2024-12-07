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
#include "desLoc.h"

// GLFW Key callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);


class Assignment : public Framework::GLFWApplication {
private:
    std::vector<Piece> player;
    std::shared_ptr<Board> board;
    TextureManager* textureManager;
    std::shared_ptr<Shader> floorShader;
    std::shared_ptr<Shader> wallShader;
    GLint floorTexture;
    GLint wallTexture;


    // Camera
    std::shared_ptr<Framework::PerspectiveCamera> camera;
    float cameraRotation = 0;
    const float cameraDistance = 3;

    // Config
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
    Board::Pos markedSquare;
    std::vector<Piece> pillars;
    std::vector<Piece> boxes;
    std::vector<DesLoc> desLoc;
    std::vector<Piece> pieces; 

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