cmake_minimum_required(VERSION 3.25)
project(Wave_FDM)

set(CMAKE_CXX_STANDARD 23)

set(SOURCE_FILES main.cpp GLAD/src/glad.c)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mavx2 -fopenmp -march=native")

find_package(glfw3 3.3 REQUIRED)
find_package(Eigen3 3.3 NO_MODULE)
include_directories(${GLFW_INCLUDE_DIRS})
link_libraries(${GLFW_LIBRARY_DIRS})

include_directories(GLAD/include)

add_executable(Wave_FDM main.cpp GLAD/src/glad.c shader.cpp shader.h Wave_FDM_Solver.h  Wave_FDM_Solver.cpp)

target_link_libraries(${PROJECT_NAME} glfw ${CMAKE_DL_LIBS})