#ifndef GLFWAPPLICATION_H
#define GLFWAPPLICATION_H

#include <string>
#include <string.h>

struct GLFWwindow;
void glfwTerminate();

namespace Framework {
    
    class GLFWApplication
    {
        protected:
            GLFWwindow* window;
            int screenHeight;
            int screenWidth;
        private:
            std::string name;
            std::string version;
        public:
            GLFWApplication(const std::string& name,  const std::string& version, const int screenWidth, const int screenHeight) {
                this->name = name;
                this->version = version;
                this->screenWidth = screenWidth;
                this->screenHeight = screenHeight;
            }
            virtual ~GLFWApplication(){
                glfwTerminate();
            }

            // Initialization 
            virtual bool Init(); // Virtual function with default behavior.

            // Run function
            virtual void Run() = 0; // Pure virtual function that must be redefined.

            //...other functions...
    };
};

#endif