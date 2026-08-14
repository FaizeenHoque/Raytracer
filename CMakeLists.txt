cmake_minimum_required(VERSION 3.20)

project(raytracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

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
        src/headers/cube.h
        src/headers/camera.h
)

target_include_directories(glad PUBLIC
        libraries/glad/include
)

# Main executable
add_executable(raytracer
        src/main.cpp
        src/headers/shaders.h
        src/shaders.cpp
        src/objects/cube.cpp
        src/camera.cpp
        src/objects/sphere.cpp
        src/headers/sphere.h
        src/renderer.cpp
        src/headers/renderer.h
        src/scene.cpp
        src/headers/scene.h
)

target_link_libraries(raytracer PRIVATE
        glad
        glfw
)

target_include_directories(raytracer PRIVATE
        libraries/glad/include
        libraries/glfw-3.5.1/include
        libraries/glm
)

## Copy shaders beside the executable
#add_custom_command(TARGET raytracer POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/shaders
#        $<TARGET_FILE_DIR:raytracer>/shaders
#)