#include "headers/scene.h"

#include <memory>
#include <random>

#include "headers/camera.h"
#include "objects/headers/Quad.h"
#include "objects/headers/Sphere.h"
#include "objects/headers/Triangle.h"

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

void Scene::Setup()
{
	camera.position.y = 4.0f;
	camera.position.z = -20.0f;

	glm::vec3 roomMin(-5.0f, 0.0f, -10.0f);
	glm::vec3 roomMax(5.0f, 8.0f, -1.0f);

	glm::vec4 redColor(0.65f, 0.05f, 0.05f, 1.0f);
	glm::vec4 whiteColor(0.73f, 0.73f, 0.73f, 1.0f);
	glm::vec4 blueColor(0.08f, 0.2f, 0.55f, 1.0f);
	glm::vec4 grayColor(0.20f, 0.20f, 0.20f, 1.0f);
	glm::vec4 floorLightColor(0.45f, 0.75f, 0.35f, 1.0f);
	glm::vec4 floorDarkColor(0.04f, 0.18f, 0.05f, 1.0f);

	glm::vec3 noEmission(0.0f);
	const glm::vec3 matteSpecularColor(1.0f);

	const glm::vec3 roomCenter = (roomMin + roomMax) * 0.5f;
	const glm::vec3 roomSize = roomMax - roomMin;
	const glm::vec3 noRotation(0.0f);

	Quad(triangleManager, glm::vec3(roomSize.y, 1.0f, roomSize.z), glm::vec3(roomMin.x, roomCenter.y, roomCenter.z), glm::vec3(0.0f, 0.0f, -90.0f), redColor);
	Quad(triangleManager, glm::vec3(roomSize.y, 1.0f, roomSize.z), glm::vec3(roomMax.x, roomCenter.y, roomCenter.z), glm::vec3(0.0f, 0.0f, 90.0f), blueColor);
	Quad(triangleManager, glm::vec3(roomSize.x, 1.0f, roomSize.y), glm::vec3(roomCenter.x, roomCenter.y, roomMax.z), glm::vec3(-90.0f, 0.0f, 0.0f), grayColor);
	Quad(triangleManager, glm::vec3(roomSize.x, 1.0f, roomSize.y), glm::vec3(roomCenter.x, roomCenter.y, roomMin.z), glm::vec3(90.0f, 0.0f, 0.0f), whiteColor);

	// Checkerboard floor tiles.
	const int checkerCols = 4;
	const int checkerRows = 4;
	const float cellWidth = roomSize.x / checkerCols;
	const float cellDepth = roomSize.z / checkerRows;
	for (int row = 0; row < checkerRows; ++row)
	{
		for (int col = 0; col < checkerCols; ++col)
		{
			const glm::vec4& cellColor = (row + col) % 2 == 0 ? floorLightColor : floorDarkColor;
			Quad(triangleManager, glm::vec3(cellWidth, 1.0f, cellDepth), glm::vec3(roomMin.x + (col + 0.5f) * cellWidth, roomMin.y, roomMin.z + (row + 0.5f) * cellDepth), noRotation, cellColor);
		}
	}

	Quad(triangleManager, glm::vec3(roomSize.x, 1.0f, roomSize.z), glm::vec3(roomCenter.x, roomMax.y, roomCenter.z), glm::vec3(180.0f, 0.0f, 0.0f), whiteColor);

	// Downward-facing emissive ceiling panel.
	Quad(triangleManager, glm::vec3(2.7f, 1.0f, 1.3f),
		 glm::vec3(0.0f, roomMax.y - 0.02f, -5.45f), glm::vec3(180.0f, 0.0f, 0.0f),
		 glm::vec4(1.0f), 45.0f, glm::vec3(1.0f, 1.0f, 0.9f));

	// Foreground subject: its focal plane is 12 units in front of the camera.
	const glm::vec4 foregroundSphereColor(1.0f, 0.0f, 0.0f, 1.0f);
	sphereManager.AddSphere(Sphere(
		glm::vec3(0.0f, 3.0f, -8.0f),
		1.05f,
		foregroundSphereColor,
		noEmission,
		0.0f,
		1.0f,
		1.0f,
		matteSpecularColor));

	std::mt19937 rng(42);
	std::uniform_real_distribution<float> xDistribution(-4.0f, 4.0f);
	std::uniform_real_distribution<float> yDistribution(1.1f, 6.7f);
	std::uniform_real_distribution<float> colourDistribution(0.2f, 0.95f);
	for (int i = 0; i < 30; ++i)
	{
		sphereManager.AddSphere(Sphere(
			glm::vec3(xDistribution(rng), yDistribution(rng), -1.55f),
			0.42f,
			glm::vec4(colourDistribution(rng), colourDistribution(rng), colourDistribution(rng), 1.0f),
			noEmission,
			0.0f,
			0.15f,
			0.1f,
			matteSpecularColor));
	}

	triangleManager.Upload();
	sphereManager.Upload();
}

void Scene::SetupParams()
{
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
