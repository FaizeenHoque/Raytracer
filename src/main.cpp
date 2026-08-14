#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>

#include "headers/camera.h"
#include "headers/shaders.h"
#include "headers/cube.h"

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracer", NULL, NULL);
    glfwSetWindowSizeLimits(window, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);

    // verify window availability
    if (!window) { std::cout << "Failed to create GLFW window" << std::endl; glfwTerminate(); return -1; }

    // make window current context
    glfwMakeContextCurrent(window);

    // load opengl
    gladLoadGL();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Initialise shader program
    Shader shaderProgram("./shaders/default.vert", "./shaders/default.frag");

	Cube cube1(
		glm::vec3(-1.5f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f)
	);

	Cube cube2(
		glm::vec3(1.5f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f)
	);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        // set clear color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    	Camera camera(shaderProgram, glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    	cube1.Draw(shaderProgram);
    	cube2.Draw(shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
}