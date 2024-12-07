# Minimum required CMake version.
cmake_minimum_required(VERSION 3.15)

add_executable(Lab1_executable_joa main.cpp) #navnet på executable og deretter navnet på fil target
target_link_libraries(Lab1_executable_joa PRIVATE glfw) #Linker nødvendige libraries fra glfw til Lab2_executable. PRIVATE vil si at det linkede librariet bare blir lenket til Lab2_executable
target_link_libraries(Lab1_executable_joa PRIVATE glad) #Linker nødvendige libraries fra glad til Lab2_executable. PRIVATE vil si at det linkede librariet bare blir lenket til Lab2_executable

