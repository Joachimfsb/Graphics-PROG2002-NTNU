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
#include "fragmentShader.glsh"
#include "vertexShader.glsh"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "RenderCommands.h"
#include "BufferLayout.h"
#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "TextureManager.h"
#include "PerspectiveCamera.h"

#ifndef TEXTURES_DIR
#define TEXTURES_DIR "./"
#endif

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
    GLint cubeTexture;
    glm::vec4 cubeColor = glm::vec4(0.0, 0.0, 0.0, 0.0);
    TextureManager* textureCubeManager;
    std::shared_ptr<PerspectiveCamera> camera;

    glm::mat4 viewMatrix;
    glm::mat4 cubeModelMatrix;
    glm::mat4 cubeInitModelMatrix;
    float cubeX = 15.0f;
    float cubeY = 15.0f;

    float ambientStrength = 1.0f;
    
    void createCube(){
        auto cubeVerticies = GeometricTools::UnitCube3D24WNormals; //data for verticies
        auto cubeIndecies = GeometricTools::UnitCube3DTopologyTriangles24;  //data for incencies

        auto vbo = std::make_shared<VertexBuffer>(cubeVerticies.data(), cubeVerticies.size() * sizeof(cubeVerticies[0]));
        BufferLayout vboLayout = {
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float3, "a_NormalVector"}
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
        cubeModelMatrix = cubeInitModelMatrix*rotateY*rotateX;
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
        textureCubeManager = TextureManager::GetInstance();
        textureCubeManager->LoadCubeMapRGBA("cube", std::string(TEXTURES_DIR) + std::string("cube_texture.jpg"), 0);

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

        cubeInitModelMatrix = glm::translate(glm::mat4(1.0f), {0.0f, 0.5f, 0.5f})*glm::scale(glm::mat4(1.0f), {0.5f, 0.5f, 0.5f});
        updateCube();

        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); //(opens a context that supports degub output, enables the debug context). OpenGL opens a context with debugging capabilities which can provide more information like warnings and error messages
        glEnable(GL_DEBUG_OUTPUT); //(enables the opengl debug output feature. this enables debug messages)
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //(indicates that synchronous debug output is wanted. when enabled debug messages are generated immediately and sent to specified debug message callback function). 
        glDebugMessageCallback(MessageCallback, nullptr); //(Function that will be called when a debug message has been made, This is used if to pass additional data if needed). Function is used to register a callback function when a debug message is created
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE); //param1: GL_DONT_CARE you accept messages from all sources, otherwise you can specify. param2: GL_DONT_CARE all types (error, warning etc.) of messages are accepted

        return true;
    }

    virtual void Run() {
        glEnable(GL_BLEND);
        double time = 0.0; //creates a time variable with the value of zero
        glfwSetTime(time); //resets the glfw time to time's value (the variable time)
        

        while(!glfwWindowShouldClose(window)){
            time = glfwGetTime();
            glfwPollEvents();
            RenderCommands::Clear();

            ambientStrength = (sin(0.2*time) + 1) / 2;
            RenderCommands::SetClearColor(glm::vec4(ambientStrength*glm::vec3(0.0f, 0.5f, 0.5f), 1.0f));

            //glDisable(GL_DEPTH_TEST); //disable or else it'll conflict with gl_blend
            cubeVAO->Bind();          
            shaderProgram->Bind();    
            glActiveTexture(GL_TEXTURE0 + textureCubeManager->GetUnitByName("cube")) ;
              
            shaderProgram->UploadUniformMatrix4("u_ModelMatrix", cubeModelMatrix);
            shaderProgram->UploadUniformMatrix4("u_ViewMatrix", camera->GetViewMatrix());
            shaderProgram->UploadUniformMatrix4("u_ProjectionMatrix", camera->GetProjectionMatrix());

            RenderCommands::SetSolidMode();
            shaderProgram->UploadUniformFloat3("u_cameraPosition", camera->GetPosition());
            shaderProgram->UploadUniformFloat("u_specularStrength", 0.5);
            shaderProgram->UploadUniformFloat3("u_lightSourcePosition", camera->GetPosition());
            shaderProgram->UploadUniformFloat("u_diffuseStrength", 0.5);
            shaderProgram->UploadUniformFloat("u_ambientStrength", ambientStrength);
            shaderProgram->UploadUniformFloat4("u_Color", cubeColor);
            RenderCommands::DrawIndex(cubeVAO, GL_TRIANGLES);

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
    if((key == GLFW_KEY_S && action == GLFW_REPEAT) || (key == GLFW_KEY_DOWN && action == GLFW_REPEAT)){
        cubeProgram.transformCube(2.0f, 0);
    }
    if((key == GLFW_KEY_W && action == GLFW_REPEAT) || (key == GLFW_KEY_UP && action == GLFW_REPEAT)){
        cubeProgram.transformCube(-2.0f, 0);
    }
    if((key == GLFW_KEY_A && action == GLFW_REPEAT) || (key == GLFW_KEY_LEFT && action == GLFW_REPEAT)){
        cubeProgram.transformCube(0, -2.0f);
    }
    if((key == GLFW_KEY_D && action == GLFW_REPEAT) || (key == GLFW_KEY_RIGHT && action == GLFW_REPEAT)){
        cubeProgram.transformCube(0, 2.0f);
    }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}