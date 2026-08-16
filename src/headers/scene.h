#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H
#include "camera.h"
#include "shaders.h"
#include "../objects/headers/Sphere.h"
#include "../objects/headers/Triangle.h"


class Scene {
public:
	Scene(Shader shaderProgram, Camera camera, float WINDOW_WIDTH, float WINDOW_HEIGHT, bool showEnvironment);

	void Setup();

	void Scene4();
	void Scene3();
	void Scene2();
	void Scene1();

	void SetupParams();
	void SetShowEnvironment(bool enabled);
	void SetMouseSensitivity(float value);
	void SetMoveSpeed(float value);
	void SetCameraYaw(float value);
	void SetFocusDistance(float value);

	Camera camera;

private:
	Shader shaderProgram;
	SphereManager sphereManager;
	TriangleManager triangleManager;
	bool showEnvironment;
	float WINDOW_WIDTH;
	float WINDOW_HEIGHT;
};

#endif
