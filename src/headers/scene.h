#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H
#include "camera.h"
#include "shaders.h"
#include "sphere.h"
#include "triangle.h"


class Scene {
public:
	Scene(Shader shaderProgram, float WINDOW_WIDTH, float WINDOW_HEIGHT);

	void Setup();
	void SetupParams();

	Camera camera;

private:
	Shader shaderProgram;
	SphereManager sphereManager;
	TriangleManager triangleManager;
	float WINDOW_WIDTH;
	float WINDOW_HEIGHT;
};

#endif
