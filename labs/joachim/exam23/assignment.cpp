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

#include <cstdlib>
#include <ctime>

#ifndef TEXTURES_DIR
#define TEXTURES_DIR "./"
#endif


using namespace Framework;

bool moveBox(std::vector<Piece>& boxes, const std::vector<Piece>& pillars, const std::vector<Piece>& pieces, const std::vector<DesLoc>& desLoc, const Board::Pos& markedSquare, const std::string& way);

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
                    case GLFW_KEY_UP:
                        {
                        bool canMove = true;
                            for (const auto& pillar : pillars) {
                                if (markedSquare.x == pillar.GetPosition().x && markedSquare.y + 1 == pillar.GetPosition().y) {
                                    canMove = false;
                                    break;
                                }
                            }       

                            if (canMove) {
                                moveBox(boxes,pillars,pieces,desLoc,markedSquare, "up");
                                    moveMarkedSquare(0, 1);
                            }
                            return true;
                        }
                    case GLFW_KEY_DOWN:
                        {
                        bool canMove = true;
                            for (const auto& pillar : pillars) {
                                if (markedSquare.x == pillar.GetPosition().x && markedSquare.y - 1 == pillar.GetPosition().y) {
                                    canMove = false;
                                    break;
                                }
                            }

                            if (canMove) {
                                moveBox(boxes,pillars,pieces,desLoc,markedSquare, "down");
                                moveMarkedSquare(0, -1);
                            }
                            return true;
                        }
                    case GLFW_KEY_LEFT:
                        {
                            bool canMove = true;
                            for (const auto& pillar : pillars) {
                                if (markedSquare.x - 1 == pillar.GetPosition().x && markedSquare.y == pillar.GetPosition().y) {
                                    canMove = false;
                                    break;
                                }
                            }           
                            if (canMove) {
                                moveBox(boxes,pillars,pieces,desLoc,markedSquare, "left");
                                moveMarkedSquare(-1, 0);
                            }
                            return true;
                        }
                    case GLFW_KEY_RIGHT:
                        {
                            bool canMove = true;
                            for (const auto& pillar : pillars) {
                                if (markedSquare.x + 1 == pillar.GetPosition().x && markedSquare.y == pillar.GetPosition().y) {
                                    canMove = false;
                                    break;
                                }
                            }

                            if (canMove) {
                                moveBox(boxes,pillars,pieces,desLoc,markedSquare, "right");
                                moveMarkedSquare(1, 0);
                            }
                            return true;
                        }
                    // Quit
                    case GLFW_KEY_Q:
                        glfwSetWindowShouldClose(window, GLFW_TRUE);
                        return true;

                    case GLFW_KEY_T:
                    {
                        texture_bool = !texture_bool;
                        if(texture_bool){
                            std::cout << "Texture ON" << std::endl;
                        }
                        else{
                            std::cout << "Texture OFF" << std::endl;
                        }
                        return true;
                    }

                    /////// KEYS THAT CAN BE HELD ////////           
                    // Move camera //
                    // Rotate
                    case GLFW_KEY_A:
                        rotateCamera(-90);
                        return false;
                    case GLFW_KEY_D:
                        rotateCamera(90);
                        return false;
                    // Zoom
                    case GLFW_KEY_W:
                        zoomCamera(-15);
                        return false;
                    case GLFW_KEY_S:
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
    if (!GLFWApplication::Init()) { // if glfw doesn't start
        return false;
    }

    // Seed the random number generator
    srand(static_cast<unsigned int>(std::time(0)));

    // OpenGL / GLFW setup
    glEnable(GL_DEPTH_TEST);
    glfwSetKeyCallback(window, keyCallback); // Input

    // Create board
    board = std::make_shared<Board>();


    // Blue team: Pieces are only placed within the inner part of the board
    for (int i = BOARD_ROWS - 10; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            // Check for border squares
            if (i == 0 || i == BOARD_ROWS - 1 || j == 0 || j == BOARD_COLS - 1) {
                // Add piece to the border square
                pieces.push_back(Piece(j, i, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)));
            }
        }
    }

    // Randomize pillars within the inner part of the board
    while (pillars.size() < 6) {
        int x = rand() % (BOARD_COLS - 2) + 1; // Ensure x is within the inner part
        int y = rand() % (BOARD_ROWS - 2) + 1; // Ensure y is within the inner part
        bool positionOccupied = false;

        // Check if the position is already occupied by another pillar
        for (const auto& pillar : pillars) {
            if (pillar.GetPosition().x == x && pillar.GetPosition().y == y) {
                positionOccupied = true;
                break;
            }
        }

        // If the position is not occupied, add the new pillar
        if (!positionOccupied) {
            pillars.push_back(Piece(static_cast<float>(x), static_cast<float>(y), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)));
        }
    }
    // Randomize pillars within the inner part of the board
    while (boxes.size() < 6) {
        int x = rand() % (BOARD_COLS - 2) + 1; // Ensure x is within the inner part
        int y = rand() % (BOARD_ROWS - 2) + 1; // Ensure y is within the inner part
        bool positionOccupied = false;

        // Check if the position is already occupied by another pillar
        for (const auto& pillar : pillars) {
            if (pillar.GetPosition().x == x && pillar.GetPosition().y == y) {
                positionOccupied = true;
                break;
            }
        }
        for (const auto& box : boxes) {
            if (box.GetPosition().x == x && box.GetPosition().y == y) {
                positionOccupied = true;
                break;
            }
        }


        // If the position is not occupied, add the new pillar
        if (!positionOccupied) {
            boxes.push_back(Piece(static_cast<float>(x), static_cast<float>(y), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
        }
    }

    while (boxes.size() < 6) {
        int x = rand() % (BOARD_COLS - 2) + 1; // Ensure x is within the inner part
        int y = rand() % (BOARD_ROWS - 2) + 1; // Ensure y is within the inner part
        bool positionOccupied = false;

        // Check if the position is already occupied by another pillar
        for (const auto& pillar : pillars) {
            if (pillar.GetPosition().x == x && pillar.GetPosition().y == y) {
                positionOccupied = true;
                break;
            }
        }
        for (const auto& box : boxes) {
            if (box.GetPosition().x == x && box.GetPosition().y == y) {
                positionOccupied = true;
                break;
            }
        }


        // If the position is not occupied, add the new pillar
        if (!positionOccupied) {
            boxes.push_back(Piece(static_cast<float>(x), static_cast<float>(y), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
        }
    }

    while (desLoc.size() < 6) {
        int x = rand() % (BOARD_COLS - 2) + 1; // Ensure x is within the inner part
        int y = rand() % (BOARD_ROWS - 2) + 1; // Ensure y is within the inner part
        bool positionOccupied = false;

        // Check if the position is already occupied by another pillar
        for (const auto& pillar : pillars) {
            if (pillar.GetPosition().x == x && pillar.GetPosition().y == y) {
                positionOccupied = true;
                break;
            }
        }
        for (const auto& box : boxes) {
            if (box.GetPosition().x == x && box.GetPosition().y == y) {
                positionOccupied = true;
                break;
            }
        }
        for (const auto& loc : desLoc) {
            if (loc.GetPosition().x == x && loc.GetPosition().y == y) {
                positionOccupied = true;
                break;
            }
        }


        // If the position is not occupied, add the new pillar
        if (!positionOccupied) {
            desLoc.push_back(DesLoc(static_cast<float>(x), static_cast<float>(y), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)));
        }
    }
    // Randomize markedSquare position
    bool placeAllowed = false;
    while (!placeAllowed) {
        int x = rand() % (BOARD_COLS - 2) + 1; // Ensure x is within the inner part
        int y = rand() % (BOARD_ROWS - 2) + 1; // Ensure y is within the inner part
        bool positionOccupied = false;

        // Check if the position is already occupied by another pillar, box, desLoc, or piece
        for (const auto& pillar : pillars) {
            if (pillar.GetPosition().x == x && pillar.GetPosition().y == y) {
                positionOccupied = true;
                break;
            }
        }
        for (const auto& box : boxes) {
            if (box.GetPosition().x == x && box.GetPosition().y == y) {
                positionOccupied = true;
                break;
            }
        }
        for (const auto& loc : desLoc) {
            if (loc.GetPosition().x == x && loc.GetPosition().y == y) {
                positionOccupied = true;
                break;
            }
        }
        for (const auto& piece : pieces) {
            if (piece.GetPosition().x == x && piece.GetPosition().y == y) {
                positionOccupied = true;
                break;
            }
        }

        // If the position is not occupied, update the markedSquare position
        if (!positionOccupied) {
            markedSquare = Board::Pos(static_cast<float>(x), static_cast<float>(y));
            placeAllowed = true;
        }
    }

    player.push_back(Piece(markedSquare.x, markedSquare.y, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));

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

    // Randomize markedSquare position
    markedSquare = Board::Pos(static_cast<float>(rand() % BOARD_COLS), static_cast<float>(rand() % BOARD_ROWS));

    return true;
}

// Main rendering loop
void Assignment::Run() {

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Background color
    RenderCommands::SetClearColor(glm::vec4(0.5, 0.5, 0.5, 1.0));

    glfwSetTime(time); // Set time

    while (!glfwWindowShouldClose(window)) {
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
        for (auto& piece : pieces) {
            piece.Draw(camera->GetViewProjectionMatrix(), nullptr);
        }

        // Loop through and draw each pillar
        for (auto& pillar : pillars) {
            pillar.Draw(camera->GetViewProjectionMatrix(), nullptr);
        }

        if (!player.empty()) {
            player[0].SetPosition(Board::Pos{markedSquare.x, markedSquare.y});
        }
        for (auto &p : player) {

            p.Draw(camera->GetViewProjectionMatrix(), nullptr);
        }

        // Loop through and draw each box
        for (auto& box : boxes) {
            bool onLocation = false;
            for (const auto& loc : desLoc) {
                if (box.GetPosition().x == loc.GetPosition().x && box.GetPosition().y == loc.GetPosition().y) {
                    onLocation = true;
                    break;
                }
            }

            // Determine the color of the box
            std::shared_ptr<glm::vec4> color = onLocation ? std::make_shared<glm::vec4>(0.0f, 0.0f, 1.0f, 1.0f) : nullptr;

            // Draw the box with the correct color
            box.Draw(camera->GetViewProjectionMatrix(), color);
        }

        // Loop through and draw each destination location
        for (auto& loc : desLoc) {
            loc.Draw(camera->GetViewProjectionMatrix(), nullptr);
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
    if (x >= BOARD_COLS-2) x = BOARD_COLS-2;
    else if (x < 1) x = 1;
    if (y >= BOARD_ROWS-1) y = BOARD_ROWS-2;
    else if (y < 1) y = 1;
    
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

}

// Returns a pointer to the piece at the given pos (nullptr if none)
Piece* Assignment::getPieceAtPos(Board::Pos p) {
    for (int i = 0; i < pieces.size(); i++) {
        if (pieces[i].GetPosition() == p)
            return &pieces[i];
    }
    return nullptr;
}

bool moveBox(std::vector<Piece>& boxes, const std::vector<Piece>& pillars, const std::vector<Piece>& pieces, const std::vector<DesLoc>& desLoc, const Board::Pos& markedSquare, const std::string& way) {
    for (auto& box : boxes) {
        bool canMove = true;
        Board::Pos newPos = box.GetPosition();

        // Determine the new position based on the direction
        if (way == "down") {
            newPos.y -= 1;
        } else if (way == "up") {
            newPos.y += 1;
        } else if (way == "left") {
            newPos.x -= 1;
        } else if (way == "right") {
            newPos.x += 1;
        }

        // Check for collisions with walls (assuming walls are at the edges of the board)
        if (newPos.x < 0 || newPos.x >= BOARD_COLS || newPos.y < 0 || newPos.y >= BOARD_ROWS) {
            canMove = false;
        }

        // Check for collisions with pillars
        for (const auto& pillar : pillars) {
            if (newPos == pillar.GetPosition()) {
                canMove = false;
                break;
            }
        }

        // Check for collisions with other pieces
        for (const auto& piece : pieces) {
            if (newPos == piece.GetPosition()) {
                canMove = false;
                break;
            }
        }

        // Check for collisions with other boxes
        for (const auto& otherBox : boxes) {
            if (&otherBox != &box && newPos == otherBox.GetPosition()) {
                canMove = false;
                break;
            }
        }

        // Move the box if there are no collisions
        if (canMove) {
            if (way == "down" && markedSquare.x == box.GetPosition().x && markedSquare.y - 1 == box.GetPosition().y) {
                box.SetPosition(newPos);
                return true;
            } else if (way == "up" && markedSquare.x == box.GetPosition().x && markedSquare.y + 1 == box.GetPosition().y) {
                box.SetPosition(newPos);
                return true;
            } else if (way == "left" && markedSquare.x - 1 == box.GetPosition().x && markedSquare.y == box.GetPosition().y) {
                box.SetPosition(newPos);
                return true;
            } else if (way == "right" && markedSquare.x + 1 == box.GetPosition().x && markedSquare.y == box.GetPosition().y) {
                box.SetPosition(newPos);
                return true;
            }
        }
    }
    return false;
}

