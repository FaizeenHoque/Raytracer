#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "headers/scene.h"
#include "headers/camera.h"
#include "headers/shaders.h"
#include "headers/renderer.h"

// Scene
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define CAMERA_FOV 45.0f
#define CAMERA_NEAR 0.01f
#define CAMERA_FAR 1000.0f
#define CAMERA_YAW -90.0f
#define CAMERA_PITCH 0.0f

#define SHOW_ENVIRONMENT false

// Controls
#define SENSITIVITY 0.0f
#define WALK_SPEED 25.0f

// Raytracing
#define MAX_BOUNCE_COUNT 15
#define NUM_RAYS_PER_PIXEL 3

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracer", NULL, NULL);
    glfwSetWindowSizeLimits(window, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(0);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    Shader shaderProgram("../shaders/default.vert", "../shaders/default.frag");
    Camera camera(shaderProgram,
    	CAMERA_FOV,
    	static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
    	CAMERA_NEAR,
    	CAMERA_FAR,
    	SENSITIVITY,
    	WALK_SPEED,
    	CAMERA_YAW,
    	CAMERA_PITCH);
    Scene scene(shaderProgram, camera, WINDOW_WIDTH, WINDOW_HEIGHT, SHOW_ENVIRONMENT);
    scene.Setup();

    // Initialize renderer
    renderer renderer(shaderProgram, WINDOW_WIDTH, WINDOW_HEIGHT);

    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        bool cameraMoved = scene.camera.Look(window, deltaTime);

        if (cameraMoved)
            renderer.ResetAccumulation();

        scene.camera.OnRenderImage(WINDOW_WIDTH, WINDOW_HEIGHT);
        renderer.Render(MAX_BOUNCE_COUNT, NUM_RAYS_PER_PIXEL);
        scene.SetupParams();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
}