#include "headers/scene.h"

#include <memory>
#include <random>

#include "headers/camera.h"
#include "objects/headers/Quad.h"
#include "objects/headers/Sphere.h"
#include "objects/headers/Triangle.h"

class Cube;

Scene::Scene(Shader shaderProgram, Camera camera, float WINDOW_WIDTH, float WINDOW_HEIGHT, bool showEnvironment)
	: shaderProgram(shaderProgram),
	  camera(camera),
	  sphereManager(shaderProgram),
	  triangleManager(shaderProgram),
	  WINDOW_WIDTH(WINDOW_WIDTH),
	  WINDOW_HEIGHT(WINDOW_HEIGHT),
	  showEnvironment(showEnvironment)
{
}

void Scene::Setup() {
	triangleManager.Upload();
	sphereManager.Upload();
}

void Scene::RebuildTriangles() {
	triangleManager.Clear();
	for (const Cube& cube : cubes) cube.Build(triangleManager);
	for (const Quad& quad : quads) quad.Build(triangleManager);
	triangleManager.Upload();
}

void Scene::SetupParams() {
	shaderProgram.Activate();

	glUniform1i(glGetUniformLocation(shaderProgram.ID, "numTriangles"), triangleManager.Count());
	glUniform1i(glGetUniformLocation(shaderProgram.ID, "numSpheres"), sphereManager.Count());

	glUniform3f(glGetUniformLocation(shaderProgram.ID, "SkyColourHorizon"), 0.95f, 0.93f, 0.9f);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "SkyColourZenith"), 0.45f, 0.6f, 0.9f);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "GroundColour"), 0.55f, 0.5f, 0.52f);

	glm::vec3 sunDir = glm::normalize(glm::vec3(0.5f, -0.1f, -0.5f));

	glUniform3f(glGetUniformLocation(shaderProgram.ID, "SunLightDirection"), sunDir.x, sunDir.y, sunDir.z);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "SunFocus"), 40.0f);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "SunIntensity"), 60.0f);

	glUniform1i(glGetUniformLocation(shaderProgram.ID, "showEnvironment"), showEnvironment);
}

void Scene::SetShowEnvironment(bool enabled) {
	showEnvironment = enabled;
}

void Scene::SetMouseSensitivity(float value) {
	camera.sensitivity = value;
}

void Scene::SetMoveSpeed(float value) {
	camera.walkSpeed = value;
}

void Scene::SetCameraYaw(float value) {
	camera.yaw = value;
}

void Scene::SetFocusDistance(float value) {
	camera.focusDistance = value;
}