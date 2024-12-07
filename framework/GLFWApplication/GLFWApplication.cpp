#include <string>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h> //always include glfw after glad
#include "GLFWApplication.h"

namespace Framework {
    bool GLFWApplication::Init() {
        // GLFW initialization code (SECTION 2)
        if (!glfwInit()){
            std::cout << "Cannot find GLFW API\n" << std::endl;
            //glfwSetErrorCallback(error_callback);
            std::cout << "\nExiting program..." << std::endl;
            return false;
        }

        //GLFWmonitor* primary = glfwGetPrimaryMonitor(); //in order to use full screen
        //glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
        window = glfwCreateWindow(screenWidth, screenHeight, name.c_str(), NULL, NULL); //if monitor has a value like glfwGetPrimaryMonitor();, the program will turn fullscreen. NULL = specified size
        if (window == NULL) {
            std::cout << "Cannot create window\n" << std::endl;
            //glfwSetErrorCallback(error_callback);
            std::cout << "\nExiting program..." << std::endl;
            return false;
        }

        // OpenGL initialization code (SECTION 3)
        
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwMakeContextCurrent(window);

        // OpenGL initialization code
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD!\n";
            glfwTerminate();
            return false;
        }

        return true;
    }
}
