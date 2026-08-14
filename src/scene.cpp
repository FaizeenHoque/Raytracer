#include "headers/scene.h"

#include <memory>

#include "headers/camera.h"
#include "headers/sphere.h"

Scene::Scene(Shader shaderProgram, float WINDOW_WIDTH, float WINDOW_HEIGHT)
	: shaderProgram(shaderProgram),
	  camera(
		  shaderProgram,
		  45.0f,
		  WINDOW_WIDTH / WINDOW_HEIGHT,
		  0.1f,
		  100.0f,
		  0.1f,
		  10.0f,
		  -90.0f,
		  0.0f
	  ),
	  sphereManager(shaderProgram),
	  WINDOW_WIDTH(WINDOW_WIDTH),
	  WINDOW_HEIGHT(WINDOW_HEIGHT) {
}

void Scene::Setup() {
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
			glm::vec3(1.0f),
			1.0f
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
}