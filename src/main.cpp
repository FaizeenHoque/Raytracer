#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#include "headers/scene.h"
#include "headers/camera.h"
#include "headers/shaders.h"
#include "headers/cube.h"
#include "headers/renderer.h"
#include "headers/sphere.h"
#include "headers/renderer.h"

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracer", NULL, NULL);
    glfwSetWindowSizeLimits(window, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    Shader shaderProgram("../shaders/default.vert", "../shaders/default.frag");
	Scene scene(shaderProgram, WINDOW_WIDTH, WINDOW_HEIGHT);
	scene.Setup();

    // Initialize renderer
    renderer renderer(shaderProgram, WINDOW_WIDTH, WINDOW_HEIGHT);

    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        bool cameraMoved = scene.camera.Look(window, deltaTime);

        if (cameraMoved)
            renderer.ResetAccumulation();

        scene.camera.OnRenderImage();
        renderer.Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
}