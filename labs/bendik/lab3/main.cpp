// #include area
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "GLFWApplication.h"
#include "GeometricTools.h"
#include "cbShaders.glsh"
#include "cubeShaders.glsh"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "RenderCommands.h"
#include "BufferLayout.h"
#include "Shader.h"

using namespace Framework;

const unsigned int rows = 8;
const unsigned int cols = 8;


struct Square {
    int x;
    int y;
    Square(int x, int y) { this->x = x, this->y = y; }
};

// MessageCallback (for debugging purposes)
void GLAPIENTRY MessageCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar* message,
                                const void* userParam ) {
    std::cerr << "GL CALLBACK:" << ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ) <<
    "type = 0x" << type <<
    ", severity = 0x" << severity <<
    ", message =" << message << "\n";
}






GLuint compileShaders(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);



class chessboardFloor : public Framework::GLFWApplication {
private:
    std::shared_ptr<VertexArray> chessboardVertexArray;
    std::shared_ptr<Shader> chessboardShader;
    std::shared_ptr<VertexArray> cubeVertexArray;
    std::shared_ptr<Shader> cubeShader;
    Square highlightedSquare = Square(1,1);

    // Transformations
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 chessboardModelMatrix;
    glm::mat4 cubeModelMatrix;
    float cubeRotX = 0.0f;
    float cubeRotY = 0.0f;

    void createChessboard() {
        // Create data structure for vertecies and color
        auto chessboardVertexes = GeometricTools::UnitGridGeometry2D<rows, cols, true>();
        auto chessboardIndecies = GeometricTools::UnitGridTopologyTriangles<rows, cols>();

        // Vertex Buffer
        auto vb = std::make_shared<VertexBuffer>(chessboardVertexes.data(), chessboardVertexes.size() * sizeof(chessboardVertexes[0])); // Vertex Buffer Object
        BufferLayout vblayout = {
            {ShaderDataType::Float2, "a_Position"},
            {ShaderDataType::Float4, "a_Color"}
        };
        vb->SetLayout(vblayout);

        // Index buffer
        auto ib = std::make_shared<IndexBuffer>(chessboardIndecies.data(), chessboardIndecies.size()); // Index Buffer Object
        
        // Vertex Array
        chessboardVertexArray = std::make_shared<VertexArray>(); // Vertex Array Object
        chessboardVertexArray->AddVertexBuffer(vb);
        chessboardVertexArray->SetIndexBuffer(ib);

    }

    void createCube() {
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
        cubeVertexArray = std::make_shared<VertexArray>(); // Vertex Array Object
        cubeVertexArray->AddVertexBuffer(vb);
        cubeVertexArray->SetIndexBuffer(ib);
    }

    void updateCubeMatrix() {        
        cubeModelMatrix = glm::rotate(
            glm::mat4(1.0f),
            glm::radians(cubeRotY),
            {0.0f, 1.0f, 0.0f}
        ) * glm::rotate(
            glm::mat4(1.0f),
            glm::radians(cubeRotX),
            {1.0f, 0.0f, 0.0f}
        );
    }




public:
    chessboardFloor(const std::string& name,  const std::string& version, const int screenWidth, const int screenHeight) : GLFWApplication(name, version, screenWidth, screenHeight) {}
    ~chessboardFloor() {
        glfwDestroyWindow(window);
    }

    virtual bool Init() {
        if (!GLFWApplication::Init()) {
            return false;
        }

        glEnable(GL_DEPTH_TEST);

        glfwSetKeyCallback(window, keyCallback);

        createCube();
        createChessboard();

        
        chessboardShader = std::make_shared<Shader>(CB_VERTEX_SHADER, CB_FRAGMENT_SHADER);
        cubeShader = std::make_shared<Shader>(CUBE_VERTEX_SHADER, CUBE_FRAGMENT_SHADER);
    
        

        // Perspective transform
        float fov = glm::radians(45.0f);
        float aspectRatio = 1.0f;
        float near = 1.0f; // Near clipping plane
        float far = -10.0f; // Far clipping plane
        projectionMatrix = glm::perspective(fov, aspectRatio, near, far);

        // View matrix - camera position and orientation
        viewMatrix = glm::lookAt(
            glm::vec3(0, 0, 5),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
        );

        ///////// CUBE ///////////
        
        updateCubeMatrix();


        ///////// CHESSBOARD ///////////
    
        // Define our three components for the model matrix
        auto scale = glm::scale(glm::mat4(1.0f), {1.0f, 1.0f, 1.0f});
        
        auto rotate = glm::rotate(
            glm::mat4(1.0f),
            glm::radians(90.0f),
            {1.0f, 0.0f, 0.0f}
        );

        auto translate = glm::translate(
            glm::mat4(1.0f),
            {0.0f, -0.4f, 3.0f}
        );

        
        chessboardModelMatrix = translate * rotate * scale;

        

        // Setting window hints
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        
        // Enable capture of debug output.
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(MessageCallback, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        return true;
    }

    virtual void Run() {
        

        RenderCommands::SetClearColor(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f));
        double t = 0.0;
        glfwSetTime(t);

        
        while(!glfwWindowShouldClose(window)) {

            t = glfwGetTime();

            glfwPollEvents();
            

            RenderCommands::Clear();

            chessboardShader->Bind();
            chessboardVertexArray->Bind();
            RenderCommands::SetSolidMode();
            // Vertex shader
            chessboardShader->UploadUniformMatrix4("u_Projection", projectionMatrix);
            chessboardShader->UploadUniformMatrix4("u_View", viewMatrix);
            chessboardShader->UploadUniformMatrix4("u_Model", chessboardModelMatrix);

            // Fragment shader
            chessboardShader->UploadUniformInt2("u_highlightedSquare", highlightedSquare.x, highlightedSquare.y);
            chessboardShader->UploadUniformInt2("u_gridLayout", rows, cols);
            RenderCommands::DrawIndex(chessboardVertexArray, GL_TRIANGLES);
            
            cubeShader->Bind();
            cubeVertexArray->Bind();
            
            // Vertex shader
            cubeShader->UploadUniformMatrix4("u_Projection", projectionMatrix);
            cubeShader->UploadUniformMatrix4("u_View", viewMatrix);
            cubeShader->UploadUniformMatrix4("u_Model", cubeModelMatrix);
            RenderCommands::SetSolidMode();
            cubeShader->UploadUniformInt1("u_Black", 0);
            RenderCommands::DrawIndex(cubeVertexArray, GL_TRIANGLES);
            RenderCommands::SetWireframeMode();
            cubeShader->UploadUniformInt1("u_Black", 1);
            RenderCommands::DrawIndex(cubeVertexArray, GL_TRIANGLES);


            glfwSwapBuffers(window);
        }
    }


    void moveHighlightedSquare(int deltaX, int deltaY) {
        int newx = highlightedSquare.x + deltaX;
        int newy = highlightedSquare.y + deltaY;

        if (newx >= (int)cols) newx = cols-1;
        else if (newx < 0) newx = 0;
        if (newy >= (int)rows) newy = rows-1;
        else if (newy < 0) newy = 0;
        
        highlightedSquare.x = newx;
        highlightedSquare.y = newy;
    }

    void rotateCube(float deltaX, float deltaY) {
        cubeRotX += deltaX;
        cubeRotY += deltaY;
        updateCubeMatrix();
        //std::cout << cubeRotX << "\n";
    }
};



chessboardFloor app("Chessboard floor", "ver1", 1000, 1000);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Chessboard marker
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        app.moveHighlightedSquare(0, 1);
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        app.moveHighlightedSquare(0, -1);
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        app.moveHighlightedSquare(-1, 0);
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        app.moveHighlightedSquare(1, 0);

    // Cube rotation
    if (key == GLFW_KEY_W && action == GLFW_REPEAT)
        app.rotateCube(-2.0f, 0);
    else if (key == GLFW_KEY_S && action == GLFW_REPEAT)
        app.rotateCube(2.0f, 0);
    else if (key == GLFW_KEY_A && action == GLFW_REPEAT)
        app.rotateCube(0, -2.0f);
    else if (key == GLFW_KEY_D && action == GLFW_REPEAT)
        app.rotateCube(0, 2.0f);
}


int main(int argc, char** argv) {

    if(!app.Init()){
        return 1;
    }
    app.Run();
    
    return 0;
}