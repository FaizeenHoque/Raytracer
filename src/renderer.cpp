#include "headers/renderer.h"

#include <chrono>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

renderer::renderer(Shader& shaderProgram, int width, int height)
    : shaderProgram(shaderProgram),
      width(width),
      height(height),
      quadVAO(0),
      quadVBO(0),
      currentIndex(0),
      numRenderedFrames(0) {
    CreateFullscreenQuad();
    CreateAccumulationBuffers();
}

renderer::~renderer() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteFramebuffers(2, accumFBO);
    glDeleteTextures(2, accumTex);
}

void renderer::CreateFullscreenQuad() {
    float quadVertices[] = {
        -1.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f,

        -1.0f, 1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f};

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void renderer::CreateAccumulationBuffers() {
    glGenFramebuffers(2, accumFBO);
    glGenTextures(2, accumTex);

    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, accumTex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, accumFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTex[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Accum FBO " << i << " incomplete!" << std::endl;
    }

    ClearAccumulationBuffers();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderer::ClearAccumulationBuffers() {
    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, accumFBO[i]);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderer::ResetAccumulation() {
    numRenderedFrames = 0;
    ClearAccumulationBuffers();
}

void renderer::Render() {
    int prevIndex = 1 - currentIndex;
    static auto lastTime = std::chrono::steady_clock::now();
    static int frameCount = 0;

    glBindFramebuffer(GL_FRAMEBUFFER, accumFBO[currentIndex]);
    glViewport(0, 0, width, height);

    shaderProgram.Activate();

    glUniform2f(glGetUniformLocation(shaderProgram.ID, "screenSize"), (float)width, (float)height);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "MaxBounceCount"), 50);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "NumRaysPerPixel"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "NumRenderedFrames"), numRenderedFrames);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, accumTex[prevIndex]);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "PreviousFrame"), 0);

    RenderFullscreenQuad();
    BlitToScreen();

    numRenderedFrames++;
    currentIndex = prevIndex;

	// FPS logger
    frameCount++;
    auto now = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(now - lastTime).count();
    if (elapsed >= 1.0)
    {
        std::cout << "FPS: " << frameCount / elapsed << std::endl;
        frameCount = 0;
        lastTime = now;
    }
}

void renderer::RenderFullscreenQuad() {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void renderer::BlitToScreen() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, accumFBO[currentIndex]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}