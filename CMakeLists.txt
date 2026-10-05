cmake_minimum_required(VERSION 3.20)

project(raytracer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencies
include(FetchContent)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.5.1
)

FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)

FetchContent_MakeAvailable(glm)

# GLAD
find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(GLAD_OUT_DIR "${CMAKE_BINARY_DIR}/glad")

execute_process(
    COMMAND ${Python3_EXECUTABLE} -m glad
        --generator=c
        --out-path=${GLAD_OUT_DIR}
        --profile=core
        --api=gl=3.3
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    RESULT_VARIABLE GLAD_RESULT
)

if(NOT GLAD_RESULT EQUAL 0)
    message(FATAL_ERROR
        "Failed to generate GLAD. Install the Python glad package with: python -m pip install glad"
    )
endif()

add_library(glad STATIC ${GLAD_OUT_DIR}/src/glad.c)

target_include_directories(glad PUBLIC
    ${GLAD_OUT_DIR}/include
)

# Raytracer
add_executable(raytracer
    src/main.cpp
    src/shaders.cpp
    src/camera.cpp
    src/renderer.cpp
    src/scene.cpp
    src/objects/Sphere.cpp
    src/objects/Triangle.cpp
    src/objects/Cube.cpp
    src/objects/Quad.cpp
)

# Include directories
target_include_directories(raytracer PRIVATE
    ${GLAD_OUT_DIR}/include
)

# Libraries
target_link_libraries(raytracer PRIVATE
    glfw
    glad
    glm::glm
)

# Copy shaders beside executable
add_custom_command(TARGET raytracer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/shaders
        $<TARGET_FILE_DIR:raytracer>/shaders
)
