#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "fragmentShader.glsh"
#include "vertexShader.glsh"
#include "GLFWApplication.h"
#include "GeometricTools.h"

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
    GLuint shaderProgram;
    std::vector<uint> chessBoardIndecies;
    GLint chessVAO;
    Square markedSquare = Square(0,0);

    GLint createChessBoard(){
        auto chessBoardVerticies = Framework::GeometricTools::UnitGridGeometry2D<rows, cols>(); //data for verticies
        chessBoardIndecies = Framework::GeometricTools::UnitGridTopologyTriangles<rows, cols>(); //data for squares/incencies

        //Create VAO
        GLuint VAO;
        glGenVertexArrays(1, &VAO); //Creates one VAO
        glBindVertexArray(VAO); //Binds/sets the vao to be the active one

        //Create VBO
        GLuint VBO;
        glGenBuffers(1, &VBO); //Creates one VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO); //binds and stores the vbo to the GL_ARRAY_BUFFER target (this tells opengl that this buffer will store vertex data for rendering)

        //Create EBO
        GLuint EBO;
        glGenBuffers(1, &EBO); //Creates one EBO (ebos stores indices which tells opengl how to reuse vertices. this reduces redundancy)(they reference vertices stored in the vbo)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //binds ebo. This specifies that the buffer will be used to store indices for indexed drawing

        glBufferData(GL_ARRAY_BUFFER, chessBoardVerticies.size() * sizeof(float), chessBoardVerticies.data(), GL_STATIC_DRAW); //uploads vertexdata in the bound vbo to the GPU (vbo is bound in GL_ARRAY_BUFFER)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, chessBoardIndecies.size() * sizeof(uint), chessBoardIndecies.data(), GL_STATIC_DRAW); //uploads index data in the bound ebo to the GPU (ebo is bound in GL_ELEMENT_ARRAY_BUFFER)

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0); //index of vertex attribute, no of components per vertex attrib (here (x,y)), data type, bool for normalization, size between consecutive attributes (here two floats), where offset starts
        //^how opengl should interperet the vertex data stored in vbo
        glEnableVertexAttribArray(0); //activates/enables the vertex attribute array for currently bound VAO

        glBindBuffer(GL_ARRAY_BUFFER, 0); //unbinds the currently bound vbo in GL_ARRAY_BUFFER
        glBindVertexArray(0); //unbinds the currently bound vao

        return VAO;
    }

    public:
    harIkkeKreativitet(const std::string& name, const std::string& version, const int screenWidth, const int screenHeight) : GLFWApplication(name, version, screenWidth, screenHeight) {}
    ~harIkkeKreativitet(){
        glUseProgram(0); //closes the active shader program
        glDeleteProgram(shaderProgram); //deletes the specified shader program
        glfwDestroyWindow(window);
    }

    virtual bool Init(){
        if(!GLFWApplication::Init()){ //if glfw doesnt start
            return false;
        }

        glfwSetKeyCallback(window, key_callback);

        chessVAO = createChessBoard(); //stores the vao returned by createChessBoard in chessVAO

        shaderProgram = CreateShader(VERTEX_PROGRAM, FRAGMENT_PROGRAM);
        if(!shaderProgram){
            return false;
        }

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); //(opens a context that supports degub output, enables the debug context). OpenGL opens a context with debugging capabilities which can provide more information like warnings and error messages
        glEnable(GL_DEBUG_OUTPUT); //(enables the opengl debug output feature. this enables debug messages)
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //(indicates that synchronous debug output is wanted. when enabled debug messages are generated immediately and sent to specified debug message callback function). 
        glDebugMessageCallback(MessageCallback, nullptr); //(Function that will be called when a debug message has been made, This is used if to pass additional data if needed). Function is used to register a callback function when a debug message is created
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE); //param1: GL_DONT_CARE you accept messages from all sources, otherwise you can specify. param2: GL_DONT_CARE all types (error, warning etc.) of messages are accepted
        //param3: GL_DONT_CARE all severities are accepted, otherwise specify low, medium, high. param4: specifies the identifier of the debug message and 0 means any identifier is accepted
        //param5: nullptr controlls all message identifiers, otherwise you specify. param6: GL_TRUE means that the messages matching previous criteria are reported back
        //function: this function filters out different debug messages based on the criteria specified in the parameters

        return true;
    }

    virtual void Run() {
        glUseProgram(shaderProgram); //starts using the shader program shaderProgram for rendering

        glClearColor(0.5f, 0.5f, 0.0f, 1.0f); //background color of the window
        double time = 0.0; //creates a time variable with the value of zero
        glfwSetTime(time); //resets the glfw time to time's value (the variable time)

        auto shaderMarkedSquare = glGetUniformLocation(shaderProgram, "u_markedSquare");               //retrieves the location of u_markedSquare
        auto shaderRowLocation = glGetUniformLocation(shaderProgram, "u_rows");                  //retrieves the location of u_row in shaderProgram
        auto shaderColumnLocation = glGetUniformLocation(shaderProgram, "u_cols");               //retrieves the location of u_col in shaderProgram
        auto shaderWindowWidthLocation = glGetUniformLocation(shaderProgram, "u_windowWidth");   //retrieves the location of u_windowWidth in shaderProgram
        auto shaderWindowHeightLocation = glGetUniformLocation(shaderProgram, "u_windowHeight"); //retrieves the location of u_windowHeight in shaderProgram

        while(!glfwWindowShouldClose(window)){
            time = glfwGetTime();
            glfwPollEvents();

            //glClear(GL_COLOR_BUFFER_BIT);
            glBindVertexArray(chessVAO);                           //binds the current vao
            glUniform2i(shaderMarkedSquare, markedSquare.x, markedSquare.y);
            glUniform1ui(shaderRowLocation, rows);                 //sets the value of u_rows in shaderProgram to the value of rows (first param which is in the shader program gets the vale made in this file)
            glUniform1ui(shaderColumnLocation, cols);              //sets the value of u_cols in shaderProgream to the value of cols (first param which is in the shader program gets the vale made in this file)
            glUniform1i(shaderWindowWidthLocation, screenWidth);   //sets the value of u_windowWidth to the value of screenWidth (first param which is in the shader program gets the vale made in this file)
            glUniform1i(shaderWindowHeightLocation, screenHeight); //sets the value of u_windowHeight to the value of screenHeight (first param which is in the shader program gets the vale made in this file)
            glDrawElements(GL_TRIANGLES, chessBoardIndecies.size(), GL_UNSIGNED_INT, (const void*)0); //Draws/renders elements to screen
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
    if(key == GLFW_KEY_DOWN && action == GLFW_PRESS){
        chessBoardProgram.newMarkedSquare(0, -1);
    }
    if(key == GLFW_KEY_UP && action == GLFW_PRESS){
        chessBoardProgram.newMarkedSquare(0, 1);
    }
    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS){
        chessBoardProgram.newMarkedSquare(-1, 0);
    }
    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
        chessBoardProgram.newMarkedSquare(1, 0);
    }
}