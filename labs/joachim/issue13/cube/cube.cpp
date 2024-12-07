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
#include "RenderCommands.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace Framework;
using uint = unsigned int;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

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

class harIkkeKreativitet : public Framework::GLFWApplication{
    private:
    std::shared_ptr<Shader> shaderProgram;
    std::shared_ptr<VertexArray> cubeVAO;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 cubeModelMatrix;
    float cubeX = 0.0f;
    float cubeY = 0.0f;
    
    void createCube(){
        auto cubeVerticies = GeometricTools::UnitCubeGeometry3D; //data for verticies
        auto cubeIndecies = GeometricTools::UnitCubeTopology3D;  //data for incencies

        auto vbo = std::make_shared<VertexBuffer>(cubeVerticies.data(), cubeVerticies.size() * sizeof(cubeVerticies[0]));
        BufferLayout vboLayout = {
            {ShaderDataType::Float3, "shader_position"},
        };
        vbo->SetLayout(vboLayout);
        
        auto ebo = std::make_shared<IndexBuffer>(cubeIndecies.data(), cubeIndecies.size());

        cubeVAO = std::make_shared<VertexArray>();
        cubeVAO->AddVertexBuffer(vbo);
        cubeVAO->SetIndexBuffer(ebo);
    }

    void updateCube(){
        auto rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(cubeY), glm::vec3(0.0f, 1.0f, 0.0f));
        auto rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(cubeX), glm::vec3(1.0f, 0.0f, 0.0f));
        cubeModelMatrix = rotateY*rotateX;
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

        glEnable(GL_DEPTH_TEST); //Has to enable this in order to not generate fragments/pixels behind others. If this is disabled, you can see the "inside" of the cube
        glfwSetKeyCallback(window, key_callback);
        createCube();

        shaderProgram = std::make_shared<Shader>(VERTEX_PROGRAM, FRAGMENT_PROGRAM);

        float fov = glm::radians(45.0); //field of view
        float aspect = 1.0;             //aspect ratio
        float near = 1.0;               //near. closer than 1.0 wont show on screen
        float far = -10;                //far. further awar than -10 wont show on screen
        projectionMatrix = glm::perspective(fov, aspect, near, far);

        viewMatrix = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); //camera position

        updateCube();

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); //(opens a context that supports degub output, enables the debug context). OpenGL opens a context with debugging capabilities which can provide more information like warnings and error messages
        glEnable(GL_DEBUG_OUTPUT); //(enables the opengl debug output feature. this enables debug messages)
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //(indicates that synchronous debug output is wanted. when enabled debug messages are generated immediately and sent to specified debug message callback function). 
        glDebugMessageCallback(MessageCallback, nullptr); //(Function that will be called when a debug message has been made, This is used if to pass additional data if needed). Function is used to register a callback function when a debug message is created
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE); //param1: GL_DONT_CARE you accept messages from all sources, otherwise you can specify. param2: GL_DONT_CARE all types (error, warning etc.) of messages are accepted

        return true;
    }

    virtual void Run() {
        RenderCommands::SetClearColor(glm::vec4(0.5f, 0.5f, 0.0f, 1.0f)); //background color of the window
        double time = 0.0; //creates a time variable with the value of zero
        glfwSetTime(time); //resets the glfw time to time's value (the variable time)

        while(!glfwWindowShouldClose(window)){
            time = glfwGetTime();
            glfwPollEvents();

            RenderCommands::Clear(); //clears the depth buffer so previous data is removed
            cubeVAO->Bind();          
            shaderProgram->Bind();     
              
            shaderProgram->UploadUniformMatrix4("u_ModelMatrix", cubeModelMatrix);
            shaderProgram->UploadUniformMatrix4("u_ViewMatrix", viewMatrix);
            shaderProgram->UploadUniformMatrix4("u_ProjectionMatrix", projectionMatrix);

            RenderCommands::SetSolidMode();
            shaderProgram->UploadUniformInt1("u_line", 0); //no lines until transformation begins with the exception of outline
            RenderCommands::DrawIndex(cubeVAO, GL_TRIANGLES);
            //^param1: specifies that the elements will be drawn as triangles. param2: how many indices that will be drawn. param3: type of the drawn indicies in element buffer. param4: where the start in element buffer is (0=beginning)
            
            RenderCommands::SetWireframeMode();
            shaderProgram->UploadUniformInt1("u_line", 1); //creates a line around the Trriangles when transforming cube
            RenderCommands::DrawIndex(cubeVAO, GL_TRIANGLES);
            //^param1: specifies that the elements will be drawn as triangles. param2: how many indices that will be drawn. param3: type of the drawn indicies in element buffer. param4: where the start in element buffer is (0=beginning)
            glfwSwapBuffers(window);
        }
    }

        void transformCube(int newX, int newY){
        cubeX += newX;
        cubeY += newY;
        updateCube();
    }
};

harIkkeKreativitet cubeProgram("cube", "1.0", 1000, 1000);

int main(){
    if(!cubeProgram.Init()){
        return -1;
    }
    cubeProgram.Run();

    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if((key == GLFW_KEY_S && action == GLFW_PRESS) || (key == GLFW_KEY_DOWN && action == GLFW_PRESS)){
        cubeProgram.transformCube(2.0f, 0);
    }
    if((key == GLFW_KEY_W && action == GLFW_PRESS) || (key == GLFW_KEY_UP && action == GLFW_PRESS)){
        cubeProgram.transformCube(-2.0f, 0);
    }
    if((key == GLFW_KEY_A && action == GLFW_PRESS) || (key == GLFW_KEY_LEFT && action == GLFW_PRESS)){
        cubeProgram.transformCube(0, -2.0f);
    }
    if((key == GLFW_KEY_D && action == GLFW_PRESS) || (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)){
        cubeProgram.transformCube(0, 2.0f);
    }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}