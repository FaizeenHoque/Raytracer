cmake_minimum_required(VERSION 3.20)

project(raytracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

add_subdirectory(
        libraries/glfw-3.5.1
        ${CMAKE_BINARY_DIR}/glfw
)

# GLAD
add_library(glad
        libraries/glad/src/glad.c
        src/headers/camera.h
        src/headers/material.h
)

target_include_directories(glad PUBLIC
        libraries/glad/include
)

# Raytracer
add_executable(raytracer
        src/main.cpp

        src/headers/shaders.h
        src/shaders.cpp

        src/camera.cpp
        src/headers/camera.h

        src/renderer.cpp
        src/headers/renderer.h

        src/scene.cpp
        src/headers/scene.h

        src/objects/Sphere.cpp
        src/objects/headers/Sphere.h

        src/objects/Triangle.cpp
        src/objects/headers/Triangle.h

        src/objects/Cube.cpp
        src/objects/headers/Cube.h

        src/objects/Quad.cpp
        src/objects/headers/Quad.h
)

# Include directories
target_include_directories(raytracer PRIVATE
        libraries/glad/include
        libraries/glfw-3.5.1/include
        libraries/glm
)

# Libraries
target_link_libraries(raytracer PRIVATE
        glad
        glfw
)

# Copy shaders beside executable
add_custom_command(TARGET raytracer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/shaders
        $<TARGET_FILE_DIR:raytracer>/shaders
)