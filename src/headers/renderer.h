#ifndef RAYTRACER_RENDERER_H
#define RAYTRACER_RENDERER_H
#include "shaders.h"


class renderer {
public:
	renderer(Shader& shaderProgram, int width, int height);
	~renderer();

	void Render();
	void ResetAccumulation();

private:
	void CreateFullscreenQuad();
	void CreateAccumulationBuffers();
	void ClearAccumulationBuffers();
	void RenderFullscreenQuad();
	void BlitToScreen();

	Shader& shaderProgram;

	int width;
	int height;

	GLuint quadVAO;
	GLuint quadVBO;

	GLuint accumFBO[2];
	GLuint accumTex[2];

	int currentIndex;
	int numRenderedFrames;
};

#endif //RAYTRACER_RENDERER_H
