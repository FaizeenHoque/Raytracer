#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H
#include "camera.h"
#include "shaders.h"
#include "../objects/headers/Cube.h"
#include "../objects/headers/Quad.h"
#include "../objects/headers/Sphere.h"
#include "../objects/headers/Triangle.h"


class Scene {
public:
	Scene(Shader shaderProgram, Camera camera, float WINDOW_WIDTH, float WINDOW_HEIGHT, bool showEnvironment);

	void Setup();
	void RebuildTriangles();

	void SetupParams();
	void SetShowEnvironment(bool enabled);
	void SetMouseSensitivity(float value);
	void SetMoveSpeed(float value);
	void SetCameraYaw(float value);
	void SetFocusDistance(float value);

	bool accumulate = false;

	Camera camera;
	SphereManager sphereManager;
	TriangleManager triangleManager;

	std::vector<Cube> cubes;
	std::vector<Quad> quads;

private:
	Shader shaderProgram;
	bool showEnvironment;
	float WINDOW_WIDTH;
	float WINDOW_HEIGHT;
};

#endif
