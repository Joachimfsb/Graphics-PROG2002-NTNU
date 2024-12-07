#include "assignment.h"

Assignment assignment("Exam", "1.0", 1024, 1024);

int main(int argc, char** argv) {

    if(!assignment.Init()){
        return -1;
    }
    assignment.Run();
    
    return 0;
}