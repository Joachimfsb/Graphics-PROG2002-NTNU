// #include area
#include <iostream>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const std::string VERTEX_SHADER = R"(
    #version 430 core

    layout(location = 0) in vec3 position;

    void main()
    {
        gl_Position = vec4(position, 1.0); // Homogeneous coordinates 3D+1
    }
    )";

const std::string FRAGMENT_SHADER = R"(
    #version 430 core

    out vec4 color;

    uniform vec4 u_Color;

    void main()
    {
        color = u_Color;
    }
    )";


GLuint compileShaders(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
GLint createTriangle(GLfloat x1, GLfloat y1,
                     GLfloat x2, GLfloat y2,
                     GLfloat x3, GLfloat y3);
GLint createSquare(GLfloat x1, GLfloat y1,
                     GLfloat x2, GLfloat y2,
                     GLfloat x3, GLfloat y3,
                     GLfloat x4, GLfloat y4);

int main(int argc, char** argv) {


    // GLFW initialization code
    if (glfwInit() != GLFW_TRUE) {
        std::cout << "Failed initialization of GLFW! Exitting ...\n";
        return 1;
    }

    //int numberOfMonitors;
    //GLFWmonitor** monitors = glfwGetMonitors(&numberOfMonitors);
    GLFWwindow *window = glfwCreateWindow(1000, 800, "Lab 1", NULL, NULL); // Create window
    if (window == NULL) {
        std::cout << "Could not create window! Exitting ...\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwMakeContextCurrent(window); // Activate OpenGL context

    // OpenGL initialization code
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD!\n";
        glfwTerminate();
        return 1;
    }

    // OpenGL data transfer code

    // Create a vertex buffer
    auto shader = compileShaders(VERTEX_SHADER, FRAGMENT_SHADER);

    std::vector<GLint> triangles;
    triangles.push_back(createTriangle(
        -0.5, -0.5,
         0.5, -0.5,
           0,  0.5));  

    std::vector<GLint> squares;
    squares.push_back(createSquare(
        -0.5, -0.5,
        -0.5,  0.5,
         0.5,  0.5,
         0.5, -0.5)); 


    // Time
    double t = 0.0;
    glfwSetTime(t);
    
    // Application loop code
    glClearColor(0.5f, 0.0f, 0.0f, 1.0f);

    while(!glfwWindowShouldClose(window)) {

        glfwPollEvents();
        
        // Time
        t = glfwGetTime();

        // Colors
        auto bgCol = (std::sin(t) / 2.0f) + 0.5f;
        auto sqCol = (std::cos(t) / 2.0f) + 0.5f;
        auto trCol = (std::tan(t) / 2.0f) + 0.5f;

        glClearColor(bgCol, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // TRIANGLES
        auto vertexColorLocation = glGetUniformLocation(shader, "u_Color");
        glUseProgram(shader); // Use shader
        for (auto square : squares) {
            glBindVertexArray(square); // Bind VAO
            glUniform4f(vertexColorLocation, 0.0f, sqCol, 0.0f, 1.0f);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)0);
        }
        for (auto triangle : triangles) {
            glBindVertexArray(triangle); // Bind VAO
            glUniform4f(vertexColorLocation, 0.0f, 0.0f, trCol, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        
        glfwSwapBuffers(window);
    }

    // Cleanup
    glUseProgram(0);
    glDeleteProgram(shader);

    // TODO CLEAN VAO

    // Termination code
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


GLuint compileShaders(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc) {

    // Compile the vertex shader
    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vss = vertexShaderSrc.c_str();
    glShaderSource(vertexShader, 1, &vss, nullptr);
    glCompileShader(vertexShader);

    

    // Compile the fragment shader
    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fss = fragmentShaderSrc.c_str();
    glShaderSource(fragmentShader, 1, &fss, nullptr);
    glCompileShader(fragmentShader);

    // Create a shader program
    auto shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    // Shader objects can be deleted once they 
    // have been linked in a shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}



GLint createTriangle(GLfloat x1, GLfloat y1,
                     GLfloat x2, GLfloat y2,
                     GLfloat x3, GLfloat y3) {

    // Create data structure for vertecies and color
    GLfloat triangle[3*3*2] = {
        x1, y1, 0.0f, 1.0f, 0.0f, 0.0f,
        x2, y2, 0.0f, 0.0f, 1.0f, 0.0f,
        x3, y3, 0.0f, 0.0f, 0.0f, 1.0f,
    };

    GLuint triangleVBO; // Vertex Buffer Object
    GLuint triangleVAO; // Vertex Array Object

    // Create VAO
    glGenVertexArrays(1, &triangleVAO); // Create VAO
    glBindVertexArray(triangleVAO); // Bind it

    // Generate VBO
    glGenBuffers(1, &triangleVBO); // Create VBO
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO); // Bind it

    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
                                // Send triangle data to VBO (GPU)

    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
                                // Bind VAO with VBO attribute 1 (positions)
    glEnableVertexAttribArray(0);

    // Colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
                                // Bind VAO with VBO attribute 1 (positions)
    glEnableVertexAttribArray(1);

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
    glBindVertexArray(0); // Unbind VAO
    glDeleteBuffers(1, &triangleVBO); // Delete ??

    return triangleVAO;
}


GLint createSquare(GLfloat x1, GLfloat y1,
                     GLfloat x2, GLfloat y2,
                     GLfloat x3, GLfloat y3,
                     GLfloat x4, GLfloat y4) {

    // Create data structure for vertecies and color
    GLfloat squareVertecies[4*3*2] = {
        x1, y1, 0.0f, 1.0f, 0.0f, 0.0f,
        x2, y2, 0.0f, 0.0f, 1.0f, 0.0f,
        x3, y3, 0.0f, 0.0f, 0.0f, 1.0f,
        x4, y4, 0.0f, 1.0f, 1.0f, 0.0f,
    };

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

    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertecies), squareVertecies, GL_STATIC_DRAW);
                                // Send square data to VBO (GPU)

    // Generate EBO
    glGenBuffers(1, &squareEBO); // Create EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO); // Bind it

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndecies), squareIndecies, GL_STATIC_DRAW);
                                // Send square data to EBO (GPU)

    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
                                // Bind VAO with VBO attribute 1 (positions)
    glEnableVertexAttribArray(0);

    // Colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
                                // Bind VAO with VBO attribute 1 (positions)
    glEnableVertexAttribArray(1);

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
    glBindVertexArray(0); // Unbind VAO
    glDeleteBuffers(1, &squareVBO); // Delete ??

    return squareVAO;

}