cmake_minimum_required(VERSION 3.26)
project(lab1gl)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab1gl main.cpp
        object.cpp
        object.h
        matrix.cpp
        transform.cpp
        camera.cpp
        camera.h
)
target_link_libraries(lab1gl sfml-graphics sfml-system sfml-window GLEW GL assimp)
