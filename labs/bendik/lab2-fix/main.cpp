// #include area
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "GLFWApplication.h"
#include "GeometricTools.h"
#include "fragmentShader.glsh"
#include "vertexShader.glsh"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
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
//GLint createTriangle();
//GLint createSquare();



class takkKhai : public Framework::GLFWApplication {
private:
    std::shared_ptr<VertexArray> checkerboardVertexArray;
    std::shared_ptr<Shader> chessboardShader;
    Square highlightedSquare = Square(1,1);

    void createCheckerboard() {
// Create data structure for vertecies and color
        auto checkerboardVertexes = GeometricTools::UnitGridGeometry2D<rows, cols>();
        auto checkerboardIndecies = GeometricTools::UnitGridTopologyTriangles<rows, cols>();

        // Vertex Buffer
        auto vb = std::make_shared<VertexBuffer>(checkerboardVertexes.data(), checkerboardVertexes.size() * sizeof(checkerboardVertexes[0])); // Vertex Buffer Object
        BufferLayout vblayout = {
            {ShaderDataType::Float2, "pos"}
        };
        vb->SetLayout(vblayout);

        // Index buffer
        auto ib = std::make_shared<IndexBuffer>(checkerboardIndecies.data(), checkerboardIndecies.size()); // Index Buffer Object
        
        // Vertex Array
        checkerboardVertexArray = std::make_shared<VertexArray>(); // Vertex Array Object
        checkerboardVertexArray->AddVertexBuffer(vb);
        checkerboardVertexArray->SetIndexBuffer(ib);

    }




public:
    takkKhai(const std::string& name,  const std::string& version, const int screenWidth, const int screenHeight) : GLFWApplication(name, version, screenWidth, screenHeight) {}
    ~takkKhai() {
        glfwDestroyWindow(window);
    }

    virtual bool Init() {
        if (!GLFWApplication::Init()) {
            return false;
        }

        glfwSetKeyCallback(window, keyCallback);

    
        createCheckerboard();
        
        chessboardShader = std::make_shared<Shader>(VERTEX_SHADER, FRAGMENT_SHADER);
    


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
        chessboardShader->Bind();

        glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
        double t = 0.0;
        glfwSetTime(t);

        
        while(!glfwWindowShouldClose(window)) {

            t = glfwGetTime();

            glfwPollEvents();

            glClear(GL_COLOR_BUFFER_BIT);

            checkerboardVertexArray->Bind();
            chessboardShader->UploadUniformInt2("u_highlightedSquare", highlightedSquare.x, highlightedSquare.y);
            chessboardShader->UploadUniformUInt1("u_rows", rows);
            chessboardShader->UploadUniformUInt1("u_cols", cols);
            chessboardShader->UploadUniformInt1("u_windowWidth", screenWidth);
            chessboardShader->UploadUniformInt1("u_windowHeight", screenHeight);
            glDrawElements(GL_TRIANGLES, checkerboardVertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, (const void*)0);
            
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
};



takkKhai app("Test", "ver1", 1000, 1000);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        app.moveHighlightedSquare(0, 1);
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        app.moveHighlightedSquare(0, -1);
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        app.moveHighlightedSquare(-1, 0);
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        app.moveHighlightedSquare(1, 0);
}


int main(int argc, char** argv) {

    if(!app.Init()){
        return 1;
    }
    app.Run();
    
    return 0;
}