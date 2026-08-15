#include "headers/scene.h"

#include <memory>

#include "headers/camera.h"
#include "headers/sphere.h"

Scene::Scene(Shader shaderProgram, float WINDOW_WIDTH, float WINDOW_HEIGHT)
	: shaderProgram(shaderProgram),
	  camera(shaderProgram, 45.0f, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f, 0.1f, 25.0f, -90.0f, 0.0f),
	  sphereManager(shaderProgram),
	  WINDOW_WIDTH(WINDOW_WIDTH),
	  WINDOW_HEIGHT(WINDOW_HEIGHT) {
}

void Scene::Setup() {
	// sphereManager.AddSphere(
	// 	Sphere(glm::vec3(-50.0f, 20.0f, 50.0f), 30.0f, glm::vec4(0.0f), glm::vec3(1.0f), 5.0f)
	// );

	sphereManager.AddSphere(
	Sphere(glm::vec3(0.0f, -120.0f, 0.0f), 120.0f, glm::vec4(0.55f, 0.55f, 0.55f, 1.0f), glm::vec3(0.0f), 0.0f)
);

	sphereManager.AddSphere(
		Sphere(glm::vec3(-9.0f, -0.01f, 0.0f), 0.4f, glm::vec4(0.08f, 0.08f, 0.10f, 1.0f), glm::vec3(0.0f), 0.0f)
	);

	sphereManager.AddSphere(
		Sphere(glm::vec3(-6.5f, 0.35f, 0.0f), 0.6f, glm::vec4(0.18f, 0.38f, 0.58f, 1.0f), glm::vec3(0.0f), 0.0f)
	);

	sphereManager.AddSphere(
		Sphere(glm::vec3(-3.5f, 1.0f, 0.0f), 1.0f, glm::vec4(0.22f, 0.52f, 0.18f, 1.0f), glm::vec3(0.0f), 0.0f)
	);

	sphereManager.AddSphere(
		Sphere(glm::vec3(0.5f, 1.4f, 0.0f), 1.4f, glm::vec4(0.55f, 0.07f, 0.10f, 1.0f), glm::vec3(0.0f), 0.0f)
	);

	sphereManager.AddSphere(
		Sphere(glm::vec3(6.0f, 2.4f, 0.0f), 2.4f, glm::vec4(0.58f, 0.64f, 0.72f, 1.0f), glm::vec3(0.0f), 0.0f)
	);

	sphereManager.Upload();
}

void Scene::SetupParams() {
	shaderProgram.Activate();
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "SkyColourHorizon"), 0.95f, 0.93f, 0.9f);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "SkyColourZenith"), 0.45f, 0.6f, 0.9f);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "GroundColour"), 0.55f, 0.5f, 0.52f);

	glm::vec3 sunDir = glm::normalize(glm::vec3(0.5f, -0.1f, -0.5f));
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "SunLightDirection"), sunDir.x, sunDir.y, sunDir.z);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "SunFocus"), 40.0f);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "SunIntensity"), 60.0f);
}
