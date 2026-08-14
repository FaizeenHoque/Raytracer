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

    // --- Accumulation ping-pong framebuffers ---
    GLuint accumFBO[2], accumTex[2];
    glGenFramebuffers(2, accumFBO);
    glGenTextures(2, accumTex);

    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, accumTex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, accumFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTex[i], 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Accum FBO " << i << " incomplete!" << std::endl;
    }
	for (int i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, accumFBO[i]);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    int currentIndex = 0;
    int numRenderedFrames = 0;

    Camera camera(shaderProgram, 45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

    SphereManager sphereManager(shaderProgram);

	// Large light source in the background
	sphereManager.AddSphere(
		Sphere(
			glm::vec3(0.0f, 3.0f, 8.0f),
			3.0f,
			glm::vec4(1.0f, 0.85f, 0.95f, 1.0f),
			glm::vec3(1.0f),
			10.0f
		)
	);

    // Large light-purple ground sphere
    sphereManager.AddSphere(
        Sphere(
            glm::vec3(0.0f, -5.0f, 0.0f),
            5.0f,
            glm::vec4(0.65f, 0.45f, 0.9f, 1.0f),
            glm::vec3(0.0f),
            0.0f
        )
    );

    // Small spheres
    sphereManager.AddSphere(
        Sphere(
            glm::vec3(-2.0f, 0.0f, 0.0f),
            1.0f,
            glm::vec4(0.2f, 1.0f, 1.0f, 1.0f),
            glm::vec3(0.0f),
            0.0f
        )
    );

    sphereManager.AddSphere(
        Sphere(
            glm::vec3(2.0f, 0.0f, 0.0f),
            1.0f,
            glm::vec4(1.0f),
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

    	bool cameraMoved = camera.Look(window, deltaTime);

    	if (cameraMoved) {
    		numRenderedFrames = 0;

    		for (int i = 0; i < 2; i++) {
    			glBindFramebuffer(GL_FRAMEBUFFER, accumFBO[i]);
    			glClear(GL_COLOR_BUFFER_BIT);
    		}

    		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    	}

    	camera.OnRenderImage();

        int prevIndex = 1 - currentIndex;

        glBindFramebuffer(GL_FRAMEBUFFER, accumFBO[currentIndex]);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        shaderProgram.Activate();

        glUniform2f(glGetUniformLocation(shaderProgram.ID, "screenSize"), (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT);
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "MaxBounceCount"), 8);
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "NumRaysPerPixel"), 2);

        glUniform1i(glGetUniformLocation(shaderProgram.ID, "NumRenderedFrames"), numRenderedFrames);

        glUniform3f(glGetUniformLocation(shaderProgram.ID, "SkyColourHorizon"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "SkyColourZenith"), 0.3f, 0.5f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "GroundColour"), 0.35f, 0.3f, 0.25f);

        glm::vec3 sunDir = glm::normalize(glm::vec3(0.3f, -0.2f, 0.5f));
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "SunLightDirection"), sunDir.x, sunDir.y, sunDir.z);

        glUniform1f(glGetUniformLocation(shaderProgram.ID, "SunFocus"), 500.0f);
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "SunIntensity"), 10.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, accumTex[prevIndex]);
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "PreviousFrame"), 0);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // blit accumulated result to the default framebuffer so it's visible
        glBindFramebuffer(GL_READ_FRAMEBUFFER, accumFBO[currentIndex]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        numRenderedFrames++;
        currentIndex = prevIndex;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shaderProgram.Delete();
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteFramebuffers(2, accumFBO);
    glDeleteTextures(2, accumTex);

    glfwDestroyWindow(window);
    glfwTerminate();
}