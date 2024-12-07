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
#include "floorShaders.glsh"
#include "cubeShaders.glsh"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "RenderCommands.h"
#include "BufferLayout.h"
#include "Shader.h"
#include "PerspectiveCamera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "TextureManager.h"

#ifndef TEXTURES_DIR
#define TEXTURES_DIR "./"
#endif

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



class lab5App : public Framework::GLFWApplication {
private:
    std::shared_ptr<VertexArray> floorVertexArray;
    std::shared_ptr<Shader> floorShader;
    std::shared_ptr<VertexArray> cubeVertexArray;
    std::shared_ptr<Shader> cubeShader;
    Square highlightedSquare = Square(1,1);
    GLint floorTexture;
    GLint cubeTexture;
    glm::vec4 cubeColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
    TextureManager* textureManager; 
    std::shared_ptr<PerspectiveCamera> camera;


    // Transformations
    glm::mat4 floorModelMatrix;
    glm::mat4 cubeInitModelMatrix;
    glm::mat4 cubeModelMatrix;
    float cubeRotX = 15.0f;
    float cubeRotY = 15.0f;

    // Lighting
    float ambientStrength = 1.0f;


    void createFloor() {
        // Create data structure for vertecies and color
        auto floorVertexes = GeometricTools::UnitGridGeometry2D<rows, cols, true, true>();
        auto floorIndecies = GeometricTools::UnitGridTopologyTriangles<rows, cols>();

        // Vertex Buffer
        auto vb = std::make_shared<VertexBuffer>(floorVertexes.data(), floorVertexes.size() * sizeof(floorVertexes[0])); // Vertex Buffer Object
        BufferLayout vblayout = {
            {ShaderDataType::Float2, "a_Position"},
            {ShaderDataType::Float4, "a_Color"},
            {ShaderDataType::Float2, "a_TextureCoords"}
        };
        vb->SetLayout(vblayout);

        // Index buffer
        auto ib = std::make_shared<IndexBuffer>(floorIndecies.data(), floorIndecies.size()); // Index Buffer Object
        
        // Vertex Array
        floorVertexArray = std::make_shared<VertexArray>(); // Vertex Array Object
        floorVertexArray->AddVertexBuffer(vb);
        floorVertexArray->SetIndexBuffer(ib);
    }

    void createCube() {
        // Create data structure for vertecies and color
        auto cubeVertices = GeometricTools::UnitCube3D24WNormals;
        auto cubeIndices = GeometricTools::UnitCube3DTopologyTriangles24;

        // Vertex Buffer
        auto vb = std::make_shared<VertexBuffer>(cubeVertices.data(), cubeVertices.size() * sizeof(cubeVertices[0])); // Vertex Buffer Object
        BufferLayout vblayout = {
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float3, "a_NormalVector"}
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
        cubeModelMatrix = cubeInitModelMatrix
          * glm::rotate(
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
    lab5App(const std::string& name,  const std::string& version, const int screenWidth, const int screenHeight) : GLFWApplication(name, version, screenWidth, screenHeight) {}
    ~lab5App() {
        glfwDestroyWindow(window);
    }

    virtual bool Init() {
        if (!GLFWApplication::Init()) {
            return false;
        }

        glEnable(GL_DEPTH_TEST);

        glfwSetKeyCallback(window, keyCallback);

        createCube();
        createFloor();

        
        floorShader = std::make_shared<Shader>(FLOOR_VERTEX_SHADER, FLOOR_FRAGMENT_SHADER);
        cubeShader = std::make_shared<Shader>(CUBE_VERTEX_SHADER, CUBE_FRAGMENT_SHADER);
    
        std::cout << "texturedir: " << TEXTURES_DIR << std::endl;
        // Textures
        textureManager = TextureManager::GetInstance();
        textureManager->LoadTexture2DRGBA("floor", std::string(TEXTURES_DIR) + std::string("floor_texture.jpeg"), 0);
        textureManager->LoadCubeMapRGBA("cube", std::string(TEXTURES_DIR) + std::string("cube_texture.jpeg"), 1);


        // Perspective transform
        PerspectiveCamera::Frustrum frustrum;
        frustrum.angle = glm::radians(60.0f);
        frustrum.height = screenHeight;
        frustrum.width = screenWidth;
        frustrum.near = 0.1f; // Near clipping plane
        frustrum.far = -10.0f; // Far clipping plane

        camera = std::make_shared<PerspectiveCamera>(
            frustrum,
            glm::vec3(0, 2, 1), // Position
            glm::vec3(0, 0, 0), // Lookat
            glm::vec3(0, 2, 2) // Upvector
        );

        ///////// CUBE ///////////

        cubeInitModelMatrix = 
            glm::translate(
            glm::mat4(1.0f),
            {0.0f, 0.0f, 0.5f})
          * glm::scale(glm::mat4(1.0f), {0.5f, 0.5f, 0.5f});
        
        updateCubeMatrix();


        ///////// FLOOR ///////////
    
        // Define our three components for the model matrix
        auto scale = glm::scale(glm::mat4(1.0f), {1.0f, 1.0f, 1.0f});
        
        floorModelMatrix = scale;

        

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
        
        // Enable blending
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        // Set the blending function: s*alpha + d(1-alpha)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        double t = 0.0;
        glfwSetTime(t);

        
        while(!glfwWindowShouldClose(window)) {

            t = glfwGetTime();

            // Update ambient light
            ambientStrength = (sin(0.2*t) + 1) / 2; // Constrain between 0 and 1

            glfwPollEvents();

            RenderCommands::SetClearColor(glm::vec4(ambientStrength * glm::vec3(0.52f, 0.81f, 0.92f), 1.0f));

            RenderCommands::Clear();

            // Floor
            floorShader->Bind();
            floorVertexArray->Bind();
            glActiveTexture(GL_TEXTURE0 + textureManager->GetUnitByName("floor")); //Specify texture unit.

            RenderCommands::SetSolidMode();
            // Vertex shader
            floorShader->UploadUniformMatrix4("u_Projection", camera->GetProjectionMatrix());
            floorShader->UploadUniformMatrix4("u_View", camera->GetViewMatrix());
            floorShader->UploadUniformMatrix4("u_Model", floorModelMatrix);

            // Fragment shader
            floorShader->UploadUniformFloat("u_ambientStrength", ambientStrength);
            floorShader->UploadUniformInt2("u_highlightedSquare", highlightedSquare.x, highlightedSquare.y);
            floorShader->UploadUniformInt2("u_gridLayout", rows, cols);
            // Draw
            RenderCommands::DrawIndex(floorVertexArray, GL_TRIANGLES);
            

            // Cube
            cubeShader->Bind();
            cubeVertexArray->Bind();
            glActiveTexture(GL_TEXTURE0 + textureManager->GetUnitByName("cube")); //Specify texture unit.

            // Vertex shader
            cubeShader->UploadUniformMatrix4("u_Projection", camera->GetProjectionMatrix());
            cubeShader->UploadUniformMatrix4("u_View", camera->GetViewMatrix());
            cubeShader->UploadUniformMatrix4("u_Model", cubeModelMatrix);
            RenderCommands::SetSolidMode();
            // Fragment shader
            cubeShader->UploadUniformFloat3("u_lightSourcePosition", glm::vec3(0.0f, 0.0f, 1.0f));
            cubeShader->UploadUniformFloat3("u_lightColor", glm::vec3(1.0, 0.0, 0.0));
            cubeShader->UploadUniformFloat3("u_cameraPosition", camera->GetPosition());
            cubeShader->UploadUniformFloat("u_ambientStrength", ambientStrength);
            cubeShader->UploadUniformFloat("u_diffuseStrength", 0.5);
            cubeShader->UploadUniformFloat("u_specularStrength", 0.5);
            cubeShader->UploadUniformFloat4("u_Color", cubeColor);
            // Draw
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


    void setCubeColor(glm::vec4 cubeColor) {
        this->cubeColor = cubeColor;
    }

};



lab5App app("Lab 5", "ver1", 1000, 1000);

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

    // Cube color
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        app.setCubeColor(glm::vec4(0.0, 0.0, 0.0, 0.0));
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        app.setCubeColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        app.setCubeColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
    }
    if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
        app.setCubeColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
    }

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