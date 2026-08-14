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
)

target_include_directories(glad PUBLIC
        libraries/glad/include
)

# Main executable
add_executable(raytracer
        main.cpp
)

target_link_libraries(raytracer PRIVATE
        glad
        glfw
)

target_include_directories(raytracer PRIVATE
        libraries/glad/include
        libraries/glfw-3.5.1/include
)