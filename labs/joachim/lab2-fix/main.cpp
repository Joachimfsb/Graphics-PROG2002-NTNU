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
using uint = unsigned int;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

constexpr uint rows = 8;
constexpr uint cols = 8;


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

GLuint CreateShader(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);

class harIkkeKreativitet : public Framework::GLFWApplication{
    private:
    std::shared_ptr<Shader> shaderProgram;
    std::shared_ptr<VertexArray> chessVAO;
    Square markedSquare = Square(0,0);

    void createChessBoard(){
        auto chessBoardVerticies = GeometricTools::UnitGridGeometry2D<rows, cols>(); //data for verticies
        auto chessBoardIndecies = GeometricTools::UnitGridTopologyTriangles<rows, cols>(); //data for squares/incencies

        auto vbo = std::make_shared<VertexBuffer>(chessBoardVerticies.data(), chessBoardVerticies.size() * sizeof(chessBoardVerticies[0]));
        BufferLayout vboLayout = {
            {ShaderDataType::Float2, "pos"}
        };
        vbo->SetLayout(vboLayout);
        
        auto ebo = std::make_shared<IndexBuffer>(chessBoardIndecies.data(), chessBoardIndecies.size());

        chessVAO = std::make_shared<VertexArray>();
        chessVAO->AddVertexBuffer(vbo);
        chessVAO->SetIndexBuffer(ebo);
    }

    public:
    harIkkeKreativitet(const std::string& name, const std::string& version, const int screenWidth, const int screenHeight) : GLFWApplication(name, version, screenWidth, screenHeight) {}
    ~harIkkeKreativitet(){
        glfwDestroyWindow(window);
    }

    virtual bool Init(){
        if(!GLFWApplication::Init()){ //if glfw doesnt start
            return false;
        }

        glfwSetKeyCallback(window, key_callback);

        createChessBoard();

        shaderProgram = std::make_shared<Shader>(VERTEX_PROGRAM, FRAGMENT_PROGRAM);

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); //(opens a context that supports degub output, enables the debug context). OpenGL opens a context with debugging capabilities which can provide more information like warnings and error messages
        glEnable(GL_DEBUG_OUTPUT); //(enables the opengl debug output feature. this enables debug messages)
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //(indicates that synchronous debug output is wanted. when enabled debug messages are generated immediately and sent to specified debug message callback function). 
        glDebugMessageCallback(MessageCallback, nullptr); //(Function that will be called when a debug message has been made, This is used if to pass additional data if needed). Function is used to register a callback function when a debug message is created
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE); //param1: GL_DONT_CARE you accept messages from all sources, otherwise you can specify. param2: GL_DONT_CARE all types (error, warning etc.) of messages are accepted


        return true;
    }

    virtual void Run() {
        shaderProgram->Bind();

        glClearColor(0.5f, 0.5f, 0.0f, 1.0f); //background color of the window
        double time = 0.0; //creates a time variable with the value of zero
        glfwSetTime(time); //resets the glfw time to time's value (the variable time)

        while(!glfwWindowShouldClose(window)){
            time = glfwGetTime();
            glfwPollEvents();

            glClear(GL_COLOR_BUFFER_BIT);
            chessVAO->Bind();                       
            shaderProgram->UploadUniformInt2("u_markedSquare", markedSquare.x, markedSquare.y);
            shaderProgram->UploadUniformUInt1("u_rows", rows);                 //sets the value of u_rows in shaderProgram to the value of rows (first param which is in the shader program gets the vale made in this file)
            shaderProgram->UploadUniformUInt1("u_cols", cols);              //sets the value of u_cols in shaderProgream to the value of cols (first param which is in the shader program gets the vale made in this file)
            shaderProgram->UploadUniformInt1("u_windowWidth", screenWidth);   //sets the value of u_windowWidth to the value of screenWidth (first param which is in the shader program gets the vale made in this file)
            shaderProgram->UploadUniformInt1("u_windowHeight", screenHeight); //sets the value of u_windowHeight to the value of screenHeight (first param which is in the shader program gets the vale made in this file)
            glDrawElements(GL_TRIANGLES, chessVAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, (const void*)0); //Draws/renders elements to screen
            //^param1: specifies that the elements will be drawn as triangles. param2: how many indices that will be drawn. param3: type of the drawn indicies in element buffer. param4: where the start in element buffer is (0=beginning)
            glfwSwapBuffers(window);
        }
    }

    void newMarkedSquare(int newX, int newY){
        int moveRow = markedSquare.x + newX;
        int moveColumn = markedSquare.y + newY;

        if(moveRow > (int)cols-1){
            moveRow = 0;
        }
        else if(moveRow < 0){
            moveRow = rows-1;
        }
        if(moveColumn > (int)rows-1){
            moveColumn = 0;
        }
        else if(moveColumn < 0){
            moveColumn = cols-1;
        }

        markedSquare.x = moveRow;
        markedSquare.y = moveColumn;
    }
};

harIkkeKreativitet chessBoardProgram("Chessboard", "1.0", 1000, 1000);

int main(){
    if(!chessBoardProgram.Init()){
        return -1;
    }
    chessBoardProgram.Run();

    return 0;
}

/**
 * @param vertexShaderSrc:   String containing the vertex shader program
 * @param fragmentShaderSrc: String containing the fragment shader program
 * @return                   Returns a shader program
 */
GLuint CreateShader(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc){
    //Convert shader source code from std::string to raw char pointer.
    auto vertexSrc = vertexShaderSrc.c_str();
    auto fragmentSrc = fragmentShaderSrc.c_str();

    //Compile vertex shader
    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);

    //Compile fragment shader
    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);

    //Create shader program
    auto shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //Shader objects can be deleted once they have been linked in a shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if((key == GLFW_KEY_S && action == GLFW_PRESS) || (key == GLFW_KEY_DOWN && action == GLFW_PRESS)){
        chessBoardProgram.newMarkedSquare(0, -1);
    }
    if((key == GLFW_KEY_W && action == GLFW_PRESS) || (key == GLFW_KEY_UP && action == GLFW_PRESS)){
        chessBoardProgram.newMarkedSquare(0, 1);
    }
    if((key == GLFW_KEY_A && action == GLFW_PRESS) || (key == GLFW_KEY_LEFT && action == GLFW_PRESS)){
        chessBoardProgram.newMarkedSquare(-1, 0);
    }
    if((key == GLFW_KEY_D && action == GLFW_PRESS) || (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)){
        chessBoardProgram.newMarkedSquare(1, 0);
    }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}