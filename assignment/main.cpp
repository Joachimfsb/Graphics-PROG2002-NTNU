#include "assignment.h"

Assignment assignment("Assignment Program", "1.0", 800, 600);

int main(int argc, char** argv) {

    if(!assignment.Init()){
        return -1;
    }
    assignment.Run();
    
    return 0;
}