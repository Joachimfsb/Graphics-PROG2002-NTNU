// #include area
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GLFWApplication.h"
#include "GeometricTools.h"
#include "fragmentShader.glsh"
#include "vertexShader.glsh"

using uint = unsigned int;

const uint rows = 8;
const uint cols = 8;


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
    GLuint shader;
    std::vector<uint> checkerboardIndecies;
    GLint checkerboardVAO;
    Square highlightedSquare = Square(1,1);

    GLint createCheckerboard() {
        // Create data structure for vertecies and color
        auto checkerboardVertexes = Framework::GeometricTools::UnitGridGeometry2D<rows, cols>();
        checkerboardIndecies = Framework::GeometricTools::UnitGridTopologyTriangles<rows, cols>();

        GLuint cbVBO; // Vertex Buffer Object
        GLuint cbEBO; // Element Buffer Object
        GLuint cbVAO; // Vertex Array Object

        // Create VAO
        glGenVertexArrays(1, &cbVAO); // Create VAO
        glBindVertexArray(cbVAO); // Bind it

        // Generate VBO
        glGenBuffers(1, &cbVBO); // Create VBO
        glBindBuffer(GL_ARRAY_BUFFER, cbVBO); // Bind it

        glBufferData(GL_ARRAY_BUFFER, checkerboardVertexes.size() * sizeof(float), checkerboardVertexes.data(), GL_STATIC_DRAW);
                                    // Send square data to VBO (GPU)

        // Generate EBO
        glGenBuffers(1, &cbEBO); // Create EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cbEBO); // Bind it

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, checkerboardIndecies.size() * sizeof(uint), checkerboardIndecies.data(), GL_STATIC_DRAW);
                                    // Send square data to EBO (GPU)

        // Positions
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
                                    // Bind VAO with VBO attribute 1 (positions)
                                    
        glEnableVertexAttribArray(0);

        // Clean up
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
        glBindVertexArray(0); // Unbind VAO
        glDeleteBuffers(1, &cbVBO); // Delete ??

        return cbVAO;
    }




public:
    takkKhai(const std::string& name,  const std::string& version, const int screenWidth, const int screenHeight) : GLFWApplication(name, version, screenWidth, screenHeight) {}
    ~takkKhai() {
        glUseProgram(0);
        glDeleteProgram(shader);
        glfwDestroyWindow(window);
    }

    virtual bool Init() {
        if (!GLFWApplication::Init()) {
            return false;
        }

        glfwSetKeyCallback(window, keyCallback);

    
        checkerboardVAO = createCheckerboard();
        
        shader = compileShaders(VERTEX_SHADER, FRAGMENT_SHADER);
        if (shader == 0) return false;


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
        glUseProgram(shader);

        glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
        double t = 0.0;
        glfwSetTime(t);

        
        auto uHighlightedSquare = glGetUniformLocation(shader, "u_highlightedSquare");
        auto uRowsLocation = glGetUniformLocation(shader, "u_rows");
        auto uColsLocation = glGetUniformLocation(shader, "u_cols");
        auto uWindowWidthLocation = glGetUniformLocation(shader, "u_windowWidth");
        auto uWindowHeightLocation = glGetUniformLocation(shader, "u_windowHeight");

        while(!glfwWindowShouldClose(window)) {

            t = glfwGetTime();

            glfwPollEvents();

            glClear(GL_COLOR_BUFFER_BIT);

            glBindVertexArray(checkerboardVAO); // Bind VAO
            glUniform2i(uHighlightedSquare, highlightedSquare.x, highlightedSquare.y);
            glUniform1ui(uRowsLocation, rows);
            glUniform1ui(uColsLocation, cols);
            glUniform1i(uWindowWidthLocation, screenWidth);
            glUniform1i(uWindowHeightLocation, screenHeight);
            glDrawElements(GL_TRIANGLES, checkerboardIndecies.size(), GL_UNSIGNED_INT, (const void*)0);
            
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


GLuint compileShaders(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc) {

    // Compile the vertex shader
    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vss = vertexShaderSrc.c_str();
    glShaderSource(vertexShader, 1, &vss, nullptr);
    glCompileShader(vertexShader);
    GLint result;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        std::cout << "ERROR: Compilation of vertex shader failed!\n";
        return 0;
    }

    

    // Compile the fragment shader
    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fss = fragmentShaderSrc.c_str();
    glShaderSource(fragmentShader, 1, &fss, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        std::cout << "ERROR: Compilation of fragment shader failed!\n";
        return 0;
    }

    // Create a shader program
    auto shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        std::cout << "ERROR: Linking of shaders failed!\n";
        return 0;
    }

    // Shader objects can be deleted once they 
    // have been linked in a shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

/*
GLint createTriangle() {

    // Create data structure for vertecies and color
    auto vertecies = GeometricTools::UnitTriangle2D;

    GLuint triangleVBO; // Vertex Buffer Object
    GLuint triangleVAO; // Vertex Array Object

    // Create VAO
    glGenVertexArrays(1, &triangleVAO); // Create VAO
    glBindVertexArray(triangleVAO); // Bind it

    // Generate VBO
    glGenBuffers(1, &triangleVBO); // Create VBO
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO); // Bind it

    glBufferData(GL_ARRAY_BUFFER, vertecies.size() * sizeof(float), vertecies.data(), GL_STATIC_DRAW);
                                // Send triangle data to VBO (GPU)

    // Positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
                                // Bind VAO with VBO attribute 1 (positions)
    glEnableVertexAttribArray(0);

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
    glBindVertexArray(0); // Unbind VAO
    glDeleteBuffers(1, &triangleVBO); // Delete ??

    return triangleVAO;
}


GLint createSquare() {

    auto squareVertecies = GeometricTools::UnitSquare2D;

    GLuint squareIndecies[2*3] = {
        0, 1, 2,
        0, 2, 3
    };

    GLuint squareVBO; // Vertex Buffer Object
    GLuint squareEBO; // Element Buffer Object
    GLuint squareVAO; // Vertex Array Object

    // Create VAO
    glGenVertexArrays(1, &squareVAO); // Create VAO
    glBindVertexArray(squareVAO); // Bind it

    // Generate VBO
    glGenBuffers(1, &squareVBO); // Create VBO
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO); // Bind it

    glBufferData(GL_ARRAY_BUFFER, squareVertecies.size() * sizeof(float), squareVertecies.data(), GL_STATIC_DRAW);
                                // Send square data to VBO (GPU)

    // Generate EBO
    glGenBuffers(1, &squareEBO); // Create EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO); // Bind it

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndecies), squareIndecies, GL_STATIC_DRAW);
                                // Send square data to EBO (GPU)

    // Positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
                                // Bind VAO with VBO attribute 1 (positions)
                                
    glEnableVertexAttribArray(0);

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
    glBindVertexArray(0); // Unbind VAO
    glDeleteBuffers(1, &squareVBO); // Delete ??

    return squareVAO;

}
*/