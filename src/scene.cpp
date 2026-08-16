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

void Scene::Setup() {
	Scene4();
	triangleManager.Upload();
	sphereManager.Upload();
}

void Scene::Scene4() {
	SetShowEnvironment(false);
	SetMouseSensitivity(0.0f);
	SetMoveSpeed(0.0f);

	camera.position.y = 4.0f;
	camera.position.z = -20.0f;
	camera.defocusStrength = 0.0f;

	glm::vec3 roomMin(-6.0f, 0.0f, -10.0f);
	glm::vec3 roomMax(6.0f, 8.0f, -1.0f);

	glm::vec4 redColor(0.65f, 0.05f, 0.05f, 1.0f);
	glm::vec4 whiteColor(0.73f, 0.73f, 0.73f, 1.0f);
	glm::vec4 blueColor(0.08f, 0.2f, 0.55f, 1.0f);
	glm::vec4 greenColor(0.05f, 0.55f, 0.12f, 1.0f);
	glm::vec4 grayColor(0.20f, 0.20f, 0.20f, 1.0f);
	glm::vec4 floorLightColor(1.0f);
	glm::vec4 floorDarkColor(0.0f);

	glm::vec3 noEmission(0.0f);
	const glm::vec3 matteSpecularColor(1.0f);

	const glm::vec3 roomCenter = (roomMin + roomMax) * 0.5f;
	const glm::vec3 roomSize = roomMax - roomMin;
	const glm::vec3 noRotation(0.0f);

	Quad(triangleManager, glm::vec3(roomSize.y, 1.0f, roomSize.z), glm::vec3(roomMin.x, roomCenter.y, roomCenter.z), glm::vec3(0.0f, 0.0f, -90.0f), redColor);
	Quad(triangleManager, glm::vec3(roomSize.y, 1.0f, roomSize.z), glm::vec3(roomMax.x, roomCenter.y, roomCenter.z), glm::vec3(0.0f, 0.0f, 90.0f), greenColor);
	Quad(triangleManager, glm::vec3(roomSize.x, 1.0f, roomSize.y), glm::vec3(roomCenter.x, roomCenter.y, roomMax.z), glm::vec3(-90.0f, 0.0f, 0.0f), whiteColor);
	Quad(triangleManager, glm::vec3(roomSize.x, 1.0f, roomSize.y), glm::vec3(roomCenter.x, roomCenter.y, roomMin.z), glm::vec3(90.0f, 0.0f, 0.0f), grayColor);

	// Checkerboard floor tiles.
	const int checkerCols = 6;
	const int checkerRows = 6;
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
	Quad(triangleManager, glm::vec3(3.5f, 1.0f, 3.5f),
		 glm::vec3(0.0f, roomMax.y - 0.02f, -5.45f), glm::vec3(180.0f, 0.0f, 0.0f),
		 glm::vec4(1.0f), 15.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	// Foreground subject: its focal plane is 12 units in front of the camera.
	const glm::vec4 foregroundSphereColor(1.0f, 1.0f, 1.0f, 1.0f);
	sphereManager.AddSphere(Sphere(
		glm::vec3(-3.7f, 3.0f, -5.0f),
		1.05f,
		foregroundSphereColor,
		noEmission,
		0.0f,
		0.0f,
		0.0f,
		matteSpecularColor));
	sphereManager.AddSphere(Sphere(
		glm::vec3(3.7f, 3.0f, -5.0f),
		1.05f,
		foregroundSphereColor,
		noEmission,
		0.0f,
		0.0f,
		0.0f,
		matteSpecularColor));
	sphereManager.AddSphere(Sphere(
		glm::vec3(-1.2f, 3.0f, -5.0f),
		1.05f,
		foregroundSphereColor,
		noEmission,
		0.0f,
		0.0f,
		0.0f,
		matteSpecularColor));
	sphereManager.AddSphere(Sphere(
		glm::vec3(1.2f, 3.0f, -5.0f),
		1.05f,
		foregroundSphereColor,
		noEmission,
		0.0f,
		0.0f,
		0.0f,
		matteSpecularColor));
}


void Scene::Scene3() {
	SetShowEnvironment(false);
	SetMouseSensitivity(0.0f);
	SetMoveSpeed(0.0f);
	SetFocusDistance(12.0f);

	camera.position.y = 5.0f;
	camera.position.z = -13.0f;
	camera.position.x = -3.0f;
	camera.pitch = 10.0f;
	camera.yaw = -110.0f;
	camera.defocusStrength = 0.0f;

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

	Quad(triangleManager, glm::vec3(roomSize.y, 1.0f, roomSize.z), glm::vec3(roomMin.x, roomCenter.y, roomCenter.z), glm::vec3(0.0f, 0.0f, -90.0f), whiteColor, 0.0f, whiteColor, 1.0f, 0.8f, whiteColor);
	Quad(triangleManager, glm::vec3(roomSize.y, 1.0f, roomSize.z), glm::vec3(roomMax.x, roomCenter.y, roomCenter.z), glm::vec3(0.0f, 0.0f, 90.0f), whiteColor, 0.0f, whiteColor, 1.0f, 1.0f, whiteColor);
	Quad(triangleManager, glm::vec3(roomSize.x, 1.0f, roomSize.y), glm::vec3(roomCenter.x, roomCenter.y, roomMax.z), glm::vec3(-90.0f, 0.0f, 0.0f), whiteColor, 0.0f, whiteColor, 1.0f, 0.8f, whiteColor);
	Quad(triangleManager, glm::vec3(roomSize.x, 1.0f, roomSize.y), glm::vec3(roomCenter.x, roomCenter.y, roomMin.z), glm::vec3(90.0f, 0.0f, 0.0f), whiteColor, 0.0f, whiteColor, 1.0f, 1.0f, whiteColor);

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
		 glm::vec4(1.0f), 15.0f, glm::vec3(1.0f, 1.0f, 0.9f));

	// Foreground subject: its focal plane is 12 units in front of the camera.
	const glm::vec4 foregroundSphereColor(1.0f, 0.0f, 0.0f, 1.0f);
	sphereManager.AddSphere(Sphere(
		glm::vec3(0.0f, 3.0f, -8.0f),
		1.05f,
		whiteColor,
		noEmission,
		0.0f,
		1.0f,
		1.0f,
		matteSpecularColor));
}

void Scene::Scene2()
{
	SetShowEnvironment(true);
	SetMouseSensitivity(0.1f);
	SetMoveSpeed(25.0f);
	SetCameraYaw(90.0f);

	// Look down the valley towards the distant hills.
	camera.position = glm::vec3(0.0f, 3.2f, 14.0f);

	const glm::vec3 noRotation(0.0f);
	const glm::vec3 noEmission(0.0f);
	const glm::vec3 whiteSpecular(1.0f);
	const glm::vec4 grassColor(0.13f, 0.34f, 0.10f, 1.0f);
	const glm::vec4 waterColor(0.03f, 0.22f, 0.34f, 1.0f);
	const glm::vec4 sandColor(0.58f, 0.43f, 0.20f, 1.0f);
	const glm::vec4 hillColor(0.10f, 0.24f, 0.14f, 1.0f);
	const glm::vec4 rockColor(0.24f, 0.25f, 0.27f, 1.0f);
	const glm::vec4 trunkColor(0.18f, 0.07f, 0.025f, 1.0f);
	const glm::vec4 foliageColor(0.035f, 0.22f, 0.07f, 1.0f);

	// Broad ground, reflective lake, and a warm strip of shoreline.
	Quad(triangleManager, glm::vec3(100.0f, 1.0f, 120.0f), glm::vec3(0.0f, 0.0f, -35.0f), noRotation, grassColor, 0.0f, noEmission, 0.05f);
	Quad(triangleManager, glm::vec3(24.0f, 1.0f, 19.0f), glm::vec3(0.0f, 0.025f, -9.5f), noRotation, waterColor, 0.0f, noEmission, 0.82f, 0.85f, whiteSpecular);
	Quad(triangleManager, glm::vec3(28.0f, 1.0f, 2.2f), glm::vec3(0.0f, 0.015f, 0.7f), noRotation, sandColor, 0.0f, noEmission, 0.15f);

	// Partly submerged spheres form a soft, layered mountain horizon.
	const glm::vec3 hillCentres[] = {
		glm::vec3(-25.0f, -10.0f, -53.0f), glm::vec3(-11.0f, -7.5f, -57.0f),
		glm::vec3(5.0f, -9.0f, -59.0f), glm::vec3(21.0f, -7.0f, -54.0f),
		glm::vec3(35.0f, -11.0f, -62.0f)};
	const float hillRadii[] = {18.0f, 14.0f, 17.0f, 15.0f, 20.0f};
	for (int i = 0; i < 5; ++i)
	{
		sphereManager.AddSphere(Sphere(hillCentres[i], hillRadii[i], hillColor, noEmission, 0.0f, 0.15f, 0.05f, whiteSpecular));
	}

	// Stylised trees frame the lake without blocking the view down the centre.
	auto addTree = [&](const glm::vec3& base, float size)
	{
		const float trunkRadius = size * 0.18f;
		sphereManager.AddSphere(Sphere(base + glm::vec3(0.0f, trunkRadius, 0.0f), trunkRadius,
			trunkColor, noEmission, 0.0f, 0.05f, 0.05f, whiteSpecular));
		sphereManager.AddSphere(Sphere(base + glm::vec3(0.0f, size * 0.82f, 0.0f), size * 0.55f,
			foliageColor, noEmission, 0.0f, 0.2f, 0.05f, whiteSpecular));
	};
	addTree(glm::vec3(-15.0f, 0.0f, 5.0f), 3.8f);
	addTree(glm::vec3(-18.0f, 0.0f, -6.0f), 4.7f);
	addTree(glm::vec3(-16.0f, 0.0f, -19.0f), 3.2f);
	addTree(glm::vec3(15.0f, 0.0f, 4.0f), 4.3f);
	addTree(glm::vec3(18.0f, 0.0f, -8.0f), 3.5f);
	addTree(glm::vec3(15.0f, 0.0f, -22.0f), 4.8f);

	// A handful of stones give the near shoreline some scale and texture.
	const glm::vec3 rocks[] = {
		glm::vec3(-8.5f, 0.35f, 2.2f), glm::vec3(8.0f, 0.28f, 1.2f),
		glm::vec3(-12.0f, 0.25f, -2.0f), glm::vec3(11.0f, 0.4f, -3.4f)};
	for (int i = 0; i < 4; ++i)
	{
		sphereManager.AddSphere(Sphere(rocks[i], 0.55f + 0.12f * i, rockColor, noEmission, 0.0f, 0.45f, 0.25f, whiteSpecular));
	}
}

void Scene::Scene1() {
	SetShowEnvironment(false);
	SetMouseSensitivity(0.0f);
	SetMoveSpeed(0.0f);

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