// #include area (SECTION 1)
#include <iostream>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h> //always include glfw after glad
#include <cmath>
#include "squareShader.h"
#include "triangleShader.h"

GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
GLuint CreateTriangle();
GLuint CreateSquare();

void CleanVAO(GLuint &vao);

int main(int argc, char** argv){

    // GLFW initialization code (SECTION 2)
    if (!glfwInit()){
        std::cout << "Cannot find GLFW API\n" << std::endl;
        //glfwSetErrorCallback(error_callback);
        std::cout << "\nExiting program..." << std::endl;
        return 0;
    }

    GLFWmonitor* primary = glfwGetPrimaryMonitor(); //in order to use full screen
    //glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
    GLFWwindow* window = glfwCreateWindow(1200, 1200, "Lab02", NULL, NULL); //if monitor has a value like glfwGetPrimaryMonitor();, the program will turn fullscreen. NULL = specified size

    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    for(int i = 0; i < count; i++){
        std::cout << "\nMonitor" << (i+1) << ":" << monitors[i] << ((monitors[i] == primary) ? " (primary)" : "") << std::endl; //prints the monitor names (for some reason the names of the monitors are always differen)
    }
    std::cout << "\n" << std::endl;

    // OpenGL initialization code (SECTION 3)
    
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwMakeContextCurrent(window);


    // OpenGL data transfer code (SECTION 4)
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    auto triangleVAO = CreateTriangle();
    auto triangleShader = CreateShader(triangleVertexShaderSrc, triangleFragmentShaderSrc);

    auto squareVAO = CreateSquare();
    auto squareShader = CreateShader(squareVertexShaderSrc, squareFragmentShaderSrc);

    bool alternate = false;
    double currentTime = 0.0;
    double lastTime = 0.0;
    glfwSetTime(0.0);

    //Background color
    //glClearColor(0.5f, 0.0f, 0.0f, 1.0f);

    // Application loop code (SECTION 5)
    while(!glfwWindowShouldClose(window)){  
        glfwPollEvents();      
    
        // Time management for alternate flag
        currentTime = glfwGetTime();
        if (currentTime - lastTime > 1.0){
            alternate = !alternate;
            lastTime = currentTime;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        auto colorValue = (sin(currentTime) / 2.0f) + 0.5f;

        // -----------------------------------------------------------------------------
        // Draw SQUARE with dynamic green value
        // -----------------------------------------------------------------------------
        // 1. Every shader and program object has a global dictionary of all its uniform variables.
        //    This step fetches the location of the uniform variable "u_Color" within the squareShaderProgram.
        auto vertexColorLocation = glGetUniformLocation(squareShader, "u_Color");

        // 2. Bind (or activate) the shader program. Once a shader program is active, we can update its uniforms.
        glUseProgram(squareShader);

        // 3. Bind the VAO. This ensures we're using the right data (vertices, indices) for our draw call.
        glBindVertexArray(squareVAO);

        // 4. Update the "u_Color" uniform value within the currently active shader program.
        //    We use the glUniform4f function because the uniform is a vec4 (4 floats).
        //    The green component is dynamically calculated based on the sine function to make it pulsate over time.
        glUniform4f(vertexColorLocation, 0.0f, colorValue, 0.0f, 1.0f);

        // 5. Issue a draw call to OpenGL. This will use the bound VAO and the active shader program.
        //    glDrawElements indicates we're using indexed drawing,
        //    and we specify GL_TRIANGLES to denote the drawing mode.
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*)0);

        // -----------------------------------------------------------------------------
        // Draw TRIANGLE with alternate flag
        // -----------------------------------------------------------------------------
        // 1. Fetch the location of the uniform variable "u_AlternateFlag" within the triangleShaderProgram.
        auto alternateFlagLocation = glGetUniformLocation(triangleShader, "u_AlternateFlag");

        // 2. Bind (or activate) the shader program for the triangle.
        glUseProgram(triangleShader);

        // 3. Bind the VAO for the triangle.
        glBindVertexArray(triangleVAO);

        // 4. Update the "u_AlternateFlag" uniform within the currently active shader program.
        //    We use glUniform1ui because the uniform is an unsigned integer.
        //    The value we pass in depends on the "alternate" flag.
        glUniform1ui(alternateFlagLocation, static_cast<unsigned int>(alternate));

        // 5. Issue a draw call to OpenGL for the triangle.
        //    glDrawArrays is used here because the triangle isn't using indexed drawing.
        //    We specify GL_TRIANGLES to denote the drawing mode.
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // -----------------------------------------------------------------------------
        // Change the color of the background
        // -----------------------------------------------------------------------------
        auto bgColor = (cos(currentTime+1) / 6.0f) + 1.0f;
        glClearColor(colorValue, bgColor, colorValue, bgColor);

        glfwSwapBuffers(window);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            break;
        }
    }

    // Cleanup
    glUseProgram(0);
    glDeleteProgram(triangleShader);
    glDeleteProgram(squareShader);

    CleanVAO(triangleVAO);
    CleanVAO(squareVAO);

    glfwTerminate();

    return 0;
}


 void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
 }

GLuint CreateSquare(){
    GLfloat squareVertex[4*3*2] =
    {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f
    };

    GLuint squareIndices[2*3] =
    {
    0, 1, 2,
    2, 3, 0
    };

    GLuint squareVBO; // Vertex Buffer Object
    GLuint squareEBO; // Element Buffer Object
    GLuint squareVAO; // Vertex Array Object

    //creating a vertex array for the square
    glGenVertexArrays(1, &squareVAO); //creates a Vertex Array Object (VAO)
    glBindVertexArray(squareVAO);     //initializes/activates the VAO

    //Creating a vertex buffer for the square
    glGenBuffers(1, &squareVBO);              //creates a Vertex Buffer Object (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO); //activates the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertex), squareVertex, GL_STATIC_DRAW);  //had to use chatGPT to find this line

    //Creating a vertex EOB for the square
    glGenBuffers(1, &squareEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndices), squareIndices, GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &squareVBO);

    return squareVAO;
}

GLuint CreateTriangle(){
    GLfloat triangleVertex[3*3*2] = { // 3 verctices by 2 coordinate components
      -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
    };

    GLuint triangleVAO; 
    GLuint triangleVBO;

    //creating a vertex array for triangle
    glGenVertexArrays(1, &triangleVAO); //creates a Vertex Array Object (VAO)
    glBindVertexArray(triangleVAO);     //initializes/activates the VAO

    //Creating a vertex buffer for triangle
    glGenBuffers(1, &triangleVBO);              //creates a Vertex Buffer Object (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO); //activates the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertex), triangleVertex, GL_STATIC_DRAW);  //had to use chatGPT to find this line

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &triangleVBO);

    return triangleVAO;
}

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

void CleanVAO(GLuint &vao){
    
}