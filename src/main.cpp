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
#include "headers/sphere.h"

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

    if (!window) { std::cout << "Failed to create GLFW window" << std::endl; glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    Shader shaderProgram("../shaders/default.vert", "../shaders/default.frag");

    float quadVertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,

        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };

    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

    Camera camera(shaderProgram, 45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	SphereManager sphereManager(shaderProgram);

	sphereManager.AddSphere(
		Sphere(
			glm::vec3(-2.0f, 0.0f, 0.0f),
			1.0f,
			glm::vec4(0.2f, 1.0f, 1.0f, 1.0f),
			glm::vec3(0.0f),
			0.0f
		)
	);

	// sphereManager.AddSphere(
	// 	Sphere(
	// 		glm::vec3(0.0f, 0.0f, 0.0f),
	// 		0.5f,
	// 		glm::vec4(0.8f, 1.0f, 0.0f, 1.0f),
	// 		glm::vec3(1.0f),
	// 		1.0f
	// 	)
	// );

	sphereManager.AddSphere(
		Sphere(
			glm::vec3(2.0f, 0.0f, 0.0f),
			1.0f,
			glm::vec4(0.2f, 1.0f, 0.3f, 1.0f),
			glm::vec3(0.0f),
			0.0f
		)
	);

	sphereManager.AddSphere(
		Sphere(
			glm::vec3(0.0f, 0.0f, 2.0f),
			1.0f,
			glm::vec4(0.8f, 1.0f, 0.0f, 1.0f),
			glm::vec3(1.0f),
			0.0f
		)
	);

	sphereManager.Upload();

	float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
    	float currentFrame = (float)glfwGetTime();
    	float deltaTime = currentFrame - lastFrame;
    	lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.OnRenderImage();
    	camera.Look(window, deltaTime);

        shaderProgram.Activate();

    	glUniform2f(glGetUniformLocation(shaderProgram.ID, "screenSize"), (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
    	glUniform1i(glGetUniformLocation(shaderProgram.ID, "MaxBounceCount"), 30);
    	glUniform1i(glGetUniformLocation(shaderProgram.ID, "NumRaysPerPixel"), 10);
    	glUniform3f(glGetUniformLocation(shaderProgram.ID, "SkyColourHorizon"), 1.0f, 1.0f, 1.0f);
    	glUniform3f(glGetUniformLocation(shaderProgram.ID, "SkyColourZenith"), 0.3f, 0.5f, 1.0f);
    	glUniform3f(glGetUniformLocation(shaderProgram.ID, "GroundColour"), 0.35f, 0.3f, 0.25f);

    	glm::vec3 sunDir = glm::normalize(glm::vec3(0.3f, -0.6f, 0.5f));
    	glUniform3f(glGetUniformLocation(shaderProgram.ID, "SunLightDirection"), sunDir.x, sunDir.y, sunDir.z);

    	glUniform1f(glGetUniformLocation(shaderProgram.ID, "SunFocus"), 500.0f);
    	glUniform1f(glGetUniformLocation(shaderProgram.ID, "SunIntensity"), 10.0f);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    glfwDestroyWindow(window);
    glfwTerminate();
}