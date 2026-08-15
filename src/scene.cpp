#include "headers/scene.h"

#include <memory>

#include "headers/camera.h"
#include "headers/sphere.h"
1#include "headers/triangle.h"

Scene::Scene(Shader shaderProgram, float WINDOW_WIDTH, float WINDOW_HEIGHT)
	: shaderProgram(shaderProgram),
	  camera(shaderProgram, 45.0f, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f, 0.1f, 25.0f, -90.0f, 0.0f),
	  sphereManager(shaderProgram),
	  triangleManager(shaderProgram),
	  WINDOW_WIDTH(WINDOW_WIDTH),
	  WINDOW_HEIGHT(WINDOW_HEIGHT) {
}

void Scene::Setup() {
	glm::vec3 cubeMin(3.0f, 0.0f, -10.0f);
	glm::vec3 cubeMax(7.0f, 4.0f, -6.0f);

	glm::vec3 frontNormal(0.0f, 0.0f, -1.0f);
	glm::vec3 backCubeNormal(0.0f, 0.0f, 1.0f);
	glm::vec3 cubeLeftNormal(-1.0f, 0.0f, 0.0f);
	glm::vec3 cubeRightNormal(1.0f, 0.0f, 0.0f);
	glm::vec3 cubeTopNormal(0.0f, 1.0f, 0.0f);
	glm::vec3 cubeBottomNormal(0.0f, -1.0f, 0.0f);

	glm::vec4 cubeColor(0.75f, 0.25f, 0.12f, 1.0f);
	glm::vec3 noEmission(0.0f);

	// Front
	triangleManager.AddTriangle(Triangle(
		glm::vec3(cubeMin.x, cubeMin.y, cubeMin.z),
		glm::vec3(cubeMax.x, cubeMin.y, cubeMin.z),
		glm::vec3(cubeMax.x, cubeMax.y, cubeMin.z),
		frontNormal, frontNormal, frontNormal,
		cubeColor, noEmission, 0.0f
	));

	triangleManager.AddTriangle(Triangle(
		glm::vec3(cubeMin.x, cubeMin.y, cubeMin.z),
		glm::vec3(cubeMax.x, cubeMax.y, cubeMin.z),
		glm::vec3(cubeMin.x, cubeMax.y, cubeMin.z),
		frontNormal, frontNormal, frontNormal,
		cubeColor, noEmission, 0.0f
	));

	// Back
	triangleManager.AddTriangle(Triangle(
		glm::vec3(cubeMin.x, cubeMin.y, cubeMax.z),
		glm::vec3(cubeMin.x, cubeMax.y, cubeMax.z),
		glm::vec3(cubeMax.x, cubeMax.y, cubeMax.z),
		backCubeNormal, backCubeNormal, backCubeNormal,
		cubeColor, noEmission, 0.0f
	));

	triangleManager.AddTriangle(Triangle(
		glm::vec3(cubeMin.x, cubeMin.y, cubeMax.z),
		glm::vec3(cubeMax.x, cubeMax.y, cubeMax.z),
		glm::vec3(cubeMax.x, cubeMin.y, cubeMax.z),
		backCubeNormal, backCubeNormal, backCubeNormal,
		cubeColor, noEmission, 0.0f
	));

	// Left
	triangleManager.AddTriangle(Triangle(
		glm::vec3(cubeMin.x, cubeMin.y, cubeMin.z),
		glm::vec3(cubeMin.x, cubeMax.y, cubeMin.z),
		glm::vec3(cubeMin.x, cubeMax.y, cubeMax.z),
		cubeLeftNormal, cubeLeftNormal, cubeLeftNormal,
		cubeColor, noEmission, 0.0f
	));

	triangleManager.AddTriangle(Triangle(
		glm::vec3(cubeMin.x, cubeMin.y, cubeMin.z),
		glm::vec3(cubeMin.x, cubeMax.y, cubeMax.z),
		glm::vec3(cubeMin.x, cubeMin.y, cubeMax.z),
		cubeLeftNormal, cubeLeftNormal, cubeLeftNormal,
		cubeColor, noEmission, 0.0f
	));

	// Right
	triangleManager.AddTriangle(Triangle(
		glm::vec3(cubeMax.x, cubeMin.y, cubeMin.z),
		glm::vec3(cubeMax.x, cubeMin.y, cubeMax.z),
		glm::vec3(cubeMax.x, cubeMax.y, cubeMax.z),
		cubeRightNormal, cubeRightNormal, cubeRightNormal,
		cubeColor, noEmission, 0.0f
	));

	triangleManager.AddTriangle(Triangle(
		glm::vec3(cubeMax.x, cubeMin.y, cubeMin.z),
		glm::vec3(cubeMax.x, cubeMax.y, cubeMax.z),
		glm::vec3(cubeMax.x, cubeMax.y, cubeMin.z),
		cubeRightNormal, cubeRightNormal, cubeRightNormal,
		cubeColor, noEmission, 0.0f
	));

	// Top
	triangleManager.AddTriangle(Triangle(
		glm::vec3(cubeMin.x, cubeMax.y, cubeMin.z),
		glm::vec3(cubeMax.x, cubeMax.y, cubeMin.z),
		glm::vec3(cubeMax.x, cubeMax.y, cubeMax.z),
		cubeTopNormal, cubeTopNormal, cubeTopNormal,
		cubeColor, noEmission, 0.0f
	));

	triangleManager.AddTriangle(Triangle(
		glm::vec3(cubeMin.x, cubeMax.y, cubeMin.z),
		glm::vec3(cubeMax.x, cubeMax.y, cubeMax.z),
		glm::vec3(cubeMin.x, cubeMax.y, cubeMax.z),
		cubeTopNormal, cubeTopNormal, cubeTopNormal,
		cubeColor, noEmission, 0.0f
	));

	// Bottom
	triangleManager.AddTriangle(Triangle(
		glm::vec3(cubeMin.x, cubeMin.y, cubeMin.z),
		glm::vec3(cubeMin.x, cubeMin.y, cubeMax.z),
		glm::vec3(cubeMax.x, cubeMin.y, cubeMax.z),
		cubeBottomNormal, cubeBottomNormal, cubeBottomNormal,
		cubeColor, noEmission, 0.0f
	));

	triangleManager.AddTriangle(Triangle(
		glm::vec3(cubeMin.x, cubeMin.y, cubeMin.z),
		glm::vec3(cubeMax.x, cubeMin.y, cubeMax.z),
		glm::vec3(cubeMax.x, cubeMin.y, cubeMin.z),
		cubeBottomNormal, cubeBottomNormal, cubeBottomNormal,
		cubeColor, noEmission, 0.0f
	));

	sphereManager.AddSphere(
		Sphere(
			glm::vec3(10.0f, 2.0f, -8.0f),
			2.0f,
			glm::vec4(0.10f, 0.35f, 0.75f, 1.0f),
			glm::vec3(0.0f),
			0.0f
		)
	);

	triangleManager.Upload();
	sphereManager.Upload();
}

void Scene::SetupParams() {
	shaderProgram.Activate();

	glUniform1i(glGetUniformLocation(shaderProgram.ID, "numTriangles"), 19);
	glUniform1i(glGetUniformLocation(shaderProgram.ID, "numSpheres"), 1);

	glUniform3f(glGetUniformLocation(shaderProgram.ID, "SkyColourHorizon"), 0.95f, 0.93f, 0.9f);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "SkyColourZenith"), 0.45f, 0.6f, 0.9f);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "GroundColour"), 0.55f, 0.5f, 0.52f);

	glm::vec3 sunDir = glm::normalize(glm::vec3(0.5f, -0.1f, -0.5f));

	glUniform3f(glGetUniformLocation(shaderProgram.ID, "SunLightDirection"), sunDir.x, sunDir.y, sunDir.z);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "SunFocus"), 40.0f);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "SunIntensity"), 60.0f);
}