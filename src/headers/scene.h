#ifndef RAYTRACER_SCENE_H
#define RAYTRACER_SCENE_H
#include "camera.h"
#include "shaders.h"
#include "sphere.h"


class Scene {
public:
	Scene(Shader shaderProgram, float WINDOW_WIDTH, float WINDOW_HEIGHT);

	void Setup();

	Camera camera;

private:
	Shader shaderProgram;
	SphereManager sphereManager;
	float WINDOW_WIDTH;
	float WINDOW_HEIGHT;
};

#endif //RAYTRACER_SCENE_H
