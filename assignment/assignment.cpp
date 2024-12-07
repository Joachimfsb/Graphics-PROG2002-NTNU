#include "assignment.h"
// #include area
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "GLFWApplication.h"
#include "VertexArray.h"
#include "RenderCommands.h"

#include "glHelpers.h"
#include "board.h"

using namespace Framework;


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Key pressed
    if (action == GLFW_PRESS) {
        // Add to list of keys down (if not already present)
        auto it = std::find(assignment.keysDown.begin(), assignment.keysDown.end(), key);
        if (it == assignment.keysDown.end()) {
            assignment.keysDown.push_back(key);
        }

    // Key released
    } else if (action == GLFW_RELEASE) {
        // Find key
        auto it = std::find(assignment.keysDown.begin(), assignment.keysDown.end(), key);
        if (it != assignment.keysDown.end()) {
            assignment.keysDown.erase(it); // Remove it if present
        }
    }
}

void Assignment::parseInput() {

    keysDown.erase(
        std::remove_if(
            keysDown.begin(),
            keysDown.end(),
            [this](int const & key) {
                switch (key) {
                    ////// SINGLE CLICK KEYS (are removed from vector) ///////
                    // Move marker
                    case GLFW_KEY_UP: case GLFW_KEY_W:
                        moveMarkedSquare(0, 1);
                        return true;
                    case GLFW_KEY_DOWN: case GLFW_KEY_S:
                        moveMarkedSquare(0, -1);
                        return true;
                    case GLFW_KEY_LEFT: case GLFW_KEY_A:
                        moveMarkedSquare(-1, 0);
                        return true;
                    case GLFW_KEY_RIGHT: case GLFW_KEY_D:
                        moveMarkedSquare(1, 0);
                        return true;
                    // Select (enter)
                    case GLFW_KEY_ENTER:
                        if (selectedPiece) movePiece(); // If a piece is selected, it should be moved to the marked square
                        else selectPiece(); // If no piece is selected, select this piece
                        return true;
                    // Quit
                    case GLFW_KEY_Q:
                        glfwSetWindowShouldClose(window, GLFW_TRUE);
                        return true;

                    /////// KEYS THAT CAN BE HELD ////////           
                    // Move camera //
                    // Rotate
                    case GLFW_KEY_H:
                        rotateCamera(-90);
                        return false;
                    case GLFW_KEY_L:
                        rotateCamera(90);
                        return false;
                    // Zoom
                    case GLFW_KEY_P:
                        zoomCamera(-15);
                        return false;
                    case GLFW_KEY_O:
                        zoomCamera(15);
                        return false;
                    default:
                        return true;
                }
            }
        ),
        keysDown.end()
    );
}

bool Assignment::Init() {
        if(!GLFWApplication::Init()){ //if glfw doesnt start
            return false;
        }

        // OpenGL / GLFW setup
        glEnable(GL_DEPTH_TEST);
        glfwSetKeyCallback(window, keyCallback); // Input
        
        // Create board

        board = std::make_shared<Board>();

        // Create pieces

        // Red team
        for (int i = 0; i <= 1; i++)
            for (int j = 0; j < BOARD_COLS; j++)
                pieces.push_back(Piece(j, i, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));

        // Blue team
        for (int i = BOARD_ROWS-2; i < BOARD_ROWS; i++)
            for (int j = 0; j < BOARD_COLS; j++)
                pieces.push_back(Piece(j, i, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)));


        // Camera
        auto position = glm::vec3(0, 0, 2); // x, y: overwritten by rotateCamera
                                            // z: determines the height
        auto lookAt = glm::vec3(0, 0, 0);
        auto upVector = glm::vec3(0, 0, 0); // Ignore: overwritten by rotateCamera

        PerspectiveCamera::Frustrum frustrum;
        frustrum.angle = glm::radians(45.0);
        frustrum.width = screenWidth;
        frustrum.height = screenHeight;
        frustrum.near = 1.0f;
        frustrum.far = -10.0f;

        camera = std::make_shared<PerspectiveCamera>(frustrum, position, lookAt, upVector);
        rotateCamera(0);

        

        // Debug
        #ifndef NDEBUG

        std::cout << "Running in debug mode." << std::endl;
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); //(opens a context that supports degub output, enables the debug context). OpenGL opens a context with debugging capabilities which can provide more information like warnings and error messages
        glEnable(GL_DEBUG_OUTPUT); //(enables the opengl debug output feature. this enables debug messages)
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //(indicates that synchronous debug output is wanted. when enabled debug messages are generated immediately and sent to specified debug message callback function). 
        glDebugMessageCallback(MessageCallback, nullptr); //(Function that will be called when a debug message has been made, This is used if to pass additional data if needed). Function is used to register a callback function when a debug message is created
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE); //param1: GL_DONT_CARE you accept messages from all sources, otherwise you can specify. param2: GL_DONT_CARE all types (error, warning etc.) of messages are accepted
        
        #endif

        return true;
}

// Main rendering loop
void Assignment::Run() {

    // Background color
    RenderCommands::SetClearColor(glm::vec4(0.5, 0.5, 0.5, 1.0));

    glfwSetTime(time); // Set time

    while(!glfwWindowShouldClose(window)) {
        // Update time
        double newtime = glfwGetTime();
        dtime = newtime - time;
        time = newtime;

        // Events
        glfwPollEvents();

        parseInput();

        // Clear screen
        RenderCommands::Clear();

        // Draw board
        board->Draw(camera->GetViewProjectionMatrix(), markedSquare);

        // Loop through and draw each piece
        for (auto &piece : pieces) {
            std::shared_ptr<glm::vec4> overrideColor;
            // If the current piece is at the selected piece or at a marked square, override its color
            if (selectedPiece && piece.GetPosition() == selectedPiece->GetPosition())
                overrideColor = std::make_shared<glm::vec4>(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
            else if (piece.GetPosition() == markedSquare)
                overrideColor = std::make_shared<glm::vec4>(0.0f, 1.0f, 0.0f, 1.0f); // Green

            piece.Draw(camera->GetViewProjectionMatrix(), overrideColor);
        }

        // Swap buffers
        glfwSwapBuffers(window);
    }
}


// Rotates the camera around origin by deltaDegrees
void Assignment::rotateCamera(int deltaDegrees) {
    auto currentpos = camera->GetPosition();
    // Calc new rotation
    cameraRotation += dtime * -deltaDegrees;
    // Calc x and y
    currentpos.x = sin(glm::radians(cameraRotation)) * cameraDistance;
    currentpos.y = cos(glm::radians(cameraRotation)) * cameraDistance;
    // Update camera pos
    camera->SetPosition(currentpos);
    // Calc and set up-vector (always 1 above pos)
    currentpos.z += 1;
    camera->SetUpVector(currentpos);
}

// Changes the zoom of the camera by deltaDegrees
void Assignment::zoomCamera(int deltaDegrees) {
    auto currentFrustrum = camera->GetFrustrum();
    // Calc new angle
    currentFrustrum.angle += glm::radians(dtime * deltaDegrees);
    // Constrain
    if (currentFrustrum.angle < 0.1745329) // <10 degrees
        currentFrustrum.angle = 0.1745329;
    else if (currentFrustrum.angle > 1.570796) // >90 degrees
        currentFrustrum.angle = 1.570796;

    // Update camera
    camera->SetFrustrum(currentFrustrum);
    
}

// Moves the marked square
void Assignment::moveMarkedSquare(int dx, int dy) {
    int x = markedSquare.x + dx; // new x
    int y = markedSquare.y + dy; // new y

    // Constrain
    if (x >= BOARD_COLS) x = BOARD_COLS-1;
    else if (x < 0) x = 0;
    if (y >= BOARD_ROWS) y = BOARD_ROWS-1;
    else if (y < 0) y = 0;
    
    // Update markedSquare
    markedSquare.x = x;
    markedSquare.y = y;
}

void Assignment::selectPiece() {
    if (selectedPiece) return; // Verify that there is no selected piece

    // Find piece at currently marked pos
    selectedPiece = getPieceAtPos(markedSquare); // Select it
}

// Moves the selected piece to the marked square (if possible)
void Assignment::movePiece() {

    if (!selectedPiece) return; // Verify that there is a selected piece

    // Check if there is a piece at the destination
    auto destinationPiece = getPieceAtPos(markedSquare);
    if (!destinationPiece) {
        selectedPiece->SetPosition(markedSquare); // Move
    }
    selectedPiece = nullptr; // Reset
}


// Returns a pointer to the piece at the given pos (nullptr if none)
Piece* Assignment::getPieceAtPos(Board::Pos p) {
    for (int i = 0; i < pieces.size(); i++) {
        if (pieces[i].GetPosition() == p)
            return &pieces[i];
    }
    return nullptr;
}