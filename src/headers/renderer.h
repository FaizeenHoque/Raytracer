#ifndef RAYTRACER_RENDERER_H
#define RAYTRACER_RENDERER_H
#include "shaders.h"


class renderer {
public:
	renderer(Shader& shaderProgram, int width, int height);
	~renderer();

	void Render();
	void ResetAccumulation();

	GLuint GetOutputTexture() const { return accumTex[1 - currentIndex]; }
	GLuint GetOutputFBO() const { return accumFBO[1 - currentIndex]; }

	int maxBounceCount;
	int numRaysPerPixel;

	// Snapshotting / Previews
	GLuint snapshotTex = 0;
	GLuint snapshotFBO = 0;
	GLuint readFBO = 0;

	bool hasSnapshot = false;
	int numRenderedFrames;

	void CreateSnapshotBuffers();
	void CaptureSnapshot();
	GLuint GetSnapshotTexture() const { return snapshotTex; }

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
};

#endif //RAYTRACER_RENDERER_H
