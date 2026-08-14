#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>

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

    	shaderProgram.Activate();

    	glm::mat4 view = glm::lookAt(
			glm::vec3(0.0f, 1.0f, 5.0f),   // camera position
			glm::vec3(0.0f, 0.0f, 0.0f),   // looking at origin
			glm::vec3(0.0f, 1.0f, 0.0f)    // up vector
		);

    	glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
			0.1f,
			100.0f
		);

    	GLuint viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
    	GLuint projLoc = glGetUniformLocation(shaderProgram.ID, "projection");
    	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    	cube1.Draw(shaderProgram);
    	cube2.Draw(shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);
    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
}