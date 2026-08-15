#include "headers/scene.h"

#include <memory>

#include "headers/camera.h"
#include "headers/sphere.h"
#include "headers/triangle.h"

Scene::Scene(Shader shaderProgram, float WINDOW_WIDTH, float WINDOW_HEIGHT)
	: shaderProgram(shaderProgram),
	  camera(shaderProgram, 45.0f, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f, 0.0f, 25.0f, -90.0f, 0.0f),
	  sphereManager(shaderProgram),
	  triangleManager(shaderProgram),
	  WINDOW_WIDTH(WINDOW_WIDTH),
	  WINDOW_HEIGHT(WINDOW_HEIGHT) {
}

void Scene::Setup() {
	camera.position.y = 4.0f;
	camera.position.z = -20.0f;

	glm::vec3 roomMin(-5.0f, 0.0f, -10.0f);
	glm::vec3 roomMax(5.0f, 8.0f, -1.0f);

	glm::vec3 leftNormal(1.0f, 0.0f, 0.0f);   // points inward
	glm::vec3 rightNormal(-1.0f, 0.0f, 0.0f);
	glm::vec3 backNormal(0.0f, 0.0f, -1.0f);
	glm::vec3 frontNormal(0.0f, 0.0f, 1.0f);
	glm::vec3 floorNormal(0.0f, 1.0f, 0.0f);
	glm::vec3 ceilNormal(0.0f, -1.0f, 0.0f);

	glm::vec4 redColor(0.65f, 0.05f, 0.05f, 1.0f);
	glm::vec4 whiteColor(0.73f, 0.73f, 0.73f, 1.0f);
	glm::vec4 grayColor(0.20f, 0.20f, 0.20f, 1.0f);
	glm::vec4 greenColor(0.08f, 0.70f, 0.05f, 1.0f);
	glm::vec4 floorLightColor(0.78f, 0.78f, 0.74f, 1.0f);
	glm::vec4 floorDarkColor(0.04f, 0.04f, 0.04f, 1.0f);
	glm::vec4 sphereColor(0.12f, 0.42f, 0.68f, 1.0f);
	glm::vec3 noEmission(0.0f);

	// Left wall (red)
	triangleManager.AddTriangle(Triangle(
		glm::vec3(roomMin.x, roomMin.y, roomMin.z),
		glm::vec3(roomMin.x, roomMin.y, roomMax.z),
		glm::vec3(roomMin.x, roomMax.y, roomMax.z),
		leftNormal, leftNormal, leftNormal,
		redColor, noEmission, 0.0f
	));
	triangleManager.AddTriangle(Triangle(
		glm::vec3(roomMin.x, roomMin.y, roomMin.z),
		glm::vec3(roomMin.x, roomMax.y, roomMax.z),
		glm::vec3(roomMin.x, roomMax.y, roomMin.z),
		leftNormal, leftNormal, leftNormal,
		redColor, noEmission, 0.0f
	));

	// Right wall (green)
	triangleManager.AddTriangle(Triangle(
		glm::vec3(roomMax.x, roomMin.y, roomMin.z),
		glm::vec3(roomMax.x, roomMax.y, roomMax.z),
		glm::vec3(roomMax.x, roomMin.y, roomMax.z),
		rightNormal, rightNormal, rightNormal,
		greenColor, noEmission, 0.0f
	));
	triangleManager.AddTriangle(Triangle(
		glm::vec3(roomMax.x, roomMin.y, roomMin.z),
		glm::vec3(roomMax.x, roomMax.y, roomMin.z),
		glm::vec3(roomMax.x, roomMax.y, roomMax.z),
		rightNormal, rightNormal, rightNormal,
		greenColor, noEmission, 0.0f
	));

	// Back wall (white)
	triangleManager.AddTriangle(Triangle(
		glm::vec3(roomMin.x, roomMin.y, roomMax.z),
		glm::vec3(roomMax.x, roomMin.y, roomMax.z),
		glm::vec3(roomMax.x, roomMax.y, roomMax.z),
		backNormal, backNormal, backNormal,
		whiteColor, noEmission, 0.0f
	));
	triangleManager.AddTriangle(Triangle(
		glm::vec3(roomMin.x, roomMin.y, roomMax.z),
		glm::vec3(roomMax.x, roomMax.y, roomMax.z),
		glm::vec3(roomMin.x, roomMax.y, roomMax.z),
		backNormal, backNormal, backNormal,
		whiteColor, noEmission, 0.0f
	));

	// Front wall: one inward-facing, one-sided quad. It closes the room for
	// rays inside it while staying invisible from its exterior/back side.
	triangleManager.AddTriangle(Triangle(
		glm::vec3(roomMin.x, roomMin.y, roomMin.z),
		glm::vec3(roomMax.x, roomMin.y, roomMin.z),
		glm::vec3(roomMax.x, roomMax.y, roomMin.z),
		frontNormal, frontNormal, frontNormal,
		grayColor, noEmission, 0.0f
	));
	triangleManager.AddTriangle(Triangle(
		glm::vec3(roomMin.x, roomMin.y, roomMin.z),
		glm::vec3(roomMax.x, roomMax.y, roomMin.z),
		glm::vec3(roomMin.x, roomMax.y, roomMin.z),
		frontNormal, frontNormal, frontNormal,
		grayColor, noEmission, 0.0f
	));

	// Floor (checkerboard, light/dark green)
	const int checkerCols = 4;
	const int checkerRows = 4;
	float cellW = (roomMax.x - roomMin.x) / checkerCols;
	float cellD = (roomMax.z - roomMin.z) / checkerRows;

	for (int row = 0; row < checkerRows; row++) {
		for (int col = 0; col < checkerCols; col++) {
			float x0 = roomMin.x + col * cellW;
			float x1 = x0 + cellW;
			float z0 = roomMin.z + row * cellD;
			float z1 = z0 + cellD;

		glm::vec4 cellColor = ((row + col) % 2 == 0) ? floorLightColor : floorDarkColor;

			triangleManager.AddTriangle(Triangle(
				glm::vec3(x0, roomMin.y, z0),
				glm::vec3(x1, roomMin.y, z0),
				glm::vec3(x1, roomMin.y, z1),
				floorNormal, floorNormal, floorNormal,
				cellColor, noEmission, 0.0f
			));
			triangleManager.AddTriangle(Triangle(
				glm::vec3(x0, roomMin.y, z0),
				glm::vec3(x1, roomMin.y, z1),
				glm::vec3(x0, roomMin.y, z1),
				floorNormal, floorNormal, floorNormal,
				cellColor, noEmission, 0.0f
			));
		}
	}

	// Ceiling (dark gray)
	triangleManager.AddTriangle(Triangle(
		glm::vec3(roomMin.x, roomMax.y, roomMin.z),
		glm::vec3(roomMax.x, roomMax.y, roomMax.z),
		glm::vec3(roomMax.x, roomMax.y, roomMin.z),
		ceilNormal, ceilNormal, ceilNormal,
		grayColor, noEmission, 0.0f
	));
	triangleManager.AddTriangle(Triangle(
		glm::vec3(roomMin.x, roomMax.y, roomMin.z),
		glm::vec3(roomMin.x, roomMax.y, roomMax.z),
		glm::vec3(roomMax.x, roomMax.y, roomMax.z),
		ceilNormal, ceilNormal, ceilNormal,
		grayColor, noEmission, 0.0f
	));

	// Ceiling light panel (emissive box with real thickness)
	glm::vec3 lightMin(-1.35f, roomMax.y - 0.01f, -6.1f);
	glm::vec3 lightMax(1.35f, roomMax.y - 0.01f, -4.8f);
	glm::vec4 lightColor(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightEmission(1.0f, 1.0f, 0.9f);
	float lightStrength = 50.0f;
	float yOffset = 0.1f;

	float lightBottomY = lightMin.y - yOffset;

	glm::vec3 sideFrontNormal(0.0f, 0.0f, -1.0f);
	glm::vec3 sideBackNormal(0.0f, 0.0f, 1.0f);
	glm::vec3 sideLeftNormal(-1.0f, 0.0f, 0.0f);
	glm::vec3 sideRightNormal(1.0f, 0.0f, 0.0f);

	// Bottom face (the visible glowing surface)
	triangleManager.AddTriangle(Triangle(
		glm::vec3(lightMin.x, lightBottomY, lightMin.z),
		glm::vec3(lightMax.x, lightBottomY, lightMax.z),
		glm::vec3(lightMax.x, lightBottomY, lightMin.z),
		ceilNormal, ceilNormal, ceilNormal,
		lightColor, lightEmission, lightStrength
	));
	triangleManager.AddTriangle(Triangle(
		glm::vec3(lightMin.x, lightBottomY, lightMin.z),
		glm::vec3(lightMin.x, lightBottomY, lightMax.z),
		glm::vec3(lightMax.x, lightBottomY, lightMax.z),
		ceilNormal, ceilNormal, ceilNormal,
		lightColor, lightEmission, lightStrength
	));

	// Front side (z = lightMin.z)
	triangleManager.AddTriangle(Triangle(
		glm::vec3(lightMin.x, lightBottomY, lightMin.z),
		glm::vec3(lightMax.x, lightBottomY, lightMin.z),
		glm::vec3(lightMax.x, roomMax.y, lightMin.z),
		sideFrontNormal, sideFrontNormal, sideFrontNormal,
		whiteColor, lightEmission, lightStrength
	));
	triangleManager.AddTriangle(Triangle(
		glm::vec3(lightMin.x, lightBottomY, lightMin.z),
		glm::vec3(lightMax.x, roomMax.y, lightMin.z),
		glm::vec3(lightMin.x, roomMax.y, lightMin.z),
		sideFrontNormal, sideFrontNormal, sideFrontNormal,
		whiteColor, lightEmission, lightStrength
	));

	// Back side (z = lightMax.z)
	triangleManager.AddTriangle(Triangle(
		glm::vec3(lightMin.x, lightBottomY, lightMax.z),
		glm::vec3(lightMin.x, roomMax.y, lightMax.z),
		glm::vec3(lightMax.x, roomMax.y, lightMax.z),
		sideBackNormal, sideBackNormal, sideBackNormal,
		whiteColor, lightEmission, lightStrength
	));
	triangleManager.AddTriangle(Triangle(
		glm::vec3(lightMin.x, lightBottomY, lightMax.z),
		glm::vec3(lightMax.x, roomMax.y, lightMax.z),
		glm::vec3(lightMax.x, lightBottomY, lightMax.z),
		sideBackNormal, sideBackNormal, sideBackNormal,
		whiteColor, lightEmission, lightStrength
	));

	// Left side (x = lightMin.x)
	triangleManager.AddTriangle(Triangle(
		glm::vec3(lightMin.x, lightBottomY, lightMin.z),
		glm::vec3(lightMin.x, roomMax.y, lightMin.z),
		glm::vec3(lightMin.x, roomMax.y, lightMax.z),
		sideLeftNormal, sideLeftNormal, sideLeftNormal,
		whiteColor, lightEmission, lightStrength
	));
	triangleManager.AddTriangle(Triangle(
		glm::vec3(lightMin.x, lightBottomY, lightMin.z),
		glm::vec3(lightMin.x, roomMax.y, lightMax.z),
		glm::vec3(lightMin.x, lightBottomY, lightMax.z),
		sideLeftNormal, sideLeftNormal, sideLeftNormal,
		whiteColor, lightEmission, lightStrength
	));

	// Right side (x = lightMax.x)
	triangleManager.AddTriangle(Triangle(
		glm::vec3(lightMax.x, lightBottomY, lightMin.z),
		glm::vec3(lightMax.x, roomMax.y, lightMax.z),
		glm::vec3(lightMax.x, roomMax.y, lightMin.z),
		sideRightNormal, sideRightNormal, sideRightNormal,
		whiteColor, lightEmission, lightStrength
	));
	triangleManager.AddTriangle(Triangle(
		glm::vec3(lightMax.x, lightBottomY, lightMin.z),
		glm::vec3(lightMax.x, lightBottomY, lightMax.z),
		glm::vec3(lightMax.x, roomMax.y, lightMax.z),
		sideRightNormal, sideRightNormal, sideRightNormal,
		whiteColor, lightEmission, lightStrength
	));

	// Two matte cuboid subjects
	auto addBox = [&](glm::vec3 minB, glm::vec3 maxB, glm::vec4 boxColor) {
		glm::vec3 n_left(-1.0f, 0.0f, 0.0f);
		glm::vec3 n_right(1.0f, 0.0f, 0.0f);
		glm::vec3 n_front(0.0f, 0.0f, -1.0f);
		glm::vec3 n_back(0.0f, 0.0f, 1.0f);
		glm::vec3 n_top(0.0f, 1.0f, 0.0f);
		glm::vec3 n_bottom(0.0f, -1.0f, 0.0f);

		// Front
		triangleManager.AddTriangle(Triangle(
			glm::vec3(minB.x, minB.y, minB.z), glm::vec3(maxB.x, minB.y, minB.z), glm::vec3(maxB.x, maxB.y, minB.z),
			n_front, n_front, n_front, boxColor, noEmission, 0.0f));
		triangleManager.AddTriangle(Triangle(
			glm::vec3(minB.x, minB.y, minB.z), glm::vec3(maxB.x, maxB.y, minB.z), glm::vec3(minB.x, maxB.y, minB.z),
			n_front, n_front, n_front, boxColor, noEmission, 0.0f));

		// Back
		triangleManager.AddTriangle(Triangle(
			glm::vec3(minB.x, minB.y, maxB.z), glm::vec3(minB.x, maxB.y, maxB.z), glm::vec3(maxB.x, maxB.y, maxB.z),
			n_back, n_back, n_back, boxColor, noEmission, 0.0f));
		triangleManager.AddTriangle(Triangle(
			glm::vec3(minB.x, minB.y, maxB.z), glm::vec3(maxB.x, maxB.y, maxB.z), glm::vec3(maxB.x, minB.y, maxB.z),
			n_back, n_back, n_back, boxColor, noEmission, 0.0f));

		// Left
		triangleManager.AddTriangle(Triangle(
			glm::vec3(minB.x, minB.y, minB.z), glm::vec3(minB.x, maxB.y, minB.z), glm::vec3(minB.x, maxB.y, maxB.z),
			n_left, n_left, n_left, boxColor, noEmission, 0.0f));
		triangleManager.AddTriangle(Triangle(
			glm::vec3(minB.x, minB.y, minB.z), glm::vec3(minB.x, maxB.y, maxB.z), glm::vec3(minB.x, minB.y, maxB.z),
			n_left, n_left, n_left, boxColor, noEmission, 0.0f));

		// Right
		triangleManager.AddTriangle(Triangle(
			glm::vec3(maxB.x, minB.y, minB.z), glm::vec3(maxB.x, minB.y, maxB.z), glm::vec3(maxB.x, maxB.y, maxB.z),
			n_right, n_right, n_right, boxColor, noEmission, 0.0f));
		triangleManager.AddTriangle(Triangle(
			glm::vec3(maxB.x, minB.y, minB.z), glm::vec3(maxB.x, maxB.y, maxB.z), glm::vec3(maxB.x, maxB.y, minB.z),
			n_right, n_right, n_right, boxColor, noEmission, 0.0f));

		// Top
		triangleManager.AddTriangle(Triangle(
			glm::vec3(minB.x, maxB.y, minB.z), glm::vec3(maxB.x, maxB.y, minB.z), glm::vec3(maxB.x, maxB.y, maxB.z),
			n_top, n_top, n_top, boxColor, noEmission, 0.0f));
		triangleManager.AddTriangle(Triangle(
			glm::vec3(minB.x, maxB.y, minB.z), glm::vec3(maxB.x, maxB.y, maxB.z), glm::vec3(minB.x, maxB.y, maxB.z),
			n_top, n_top, n_top, boxColor, noEmission, 0.0f));

		// Bottom
		triangleManager.AddTriangle(Triangle(
			glm::vec3(minB.x, minB.y, minB.z), glm::vec3(maxB.x, minB.y, maxB.z), glm::vec3(maxB.x, minB.y, minB.z),
			n_bottom, n_bottom, n_bottom, boxColor, noEmission, 0.0f));
		triangleManager.AddTriangle(Triangle(
			glm::vec3(minB.x, minB.y, minB.z), glm::vec3(minB.x, minB.y, maxB.z), glm::vec3(minB.x, minB.y, maxB.z),
			n_bottom, n_bottom, n_bottom, boxColor, noEmission, 0.0f));
	};

	// Four matte blue spheres across the center of the room.
	const float sphereRadius = 1.05f;
	const float sphereSmoothness = 1.0f;
	const float sphereY = 3.0f;
	const float sphereZ = -5.2f;
	for (float x : {-3.1f, -1.05f, 1.05f, 3.1f}) {
		sphereManager.AddSphere(Sphere(
			glm::vec3(x, sphereY, sphereZ),
			sphereRadius,
			whiteColor,
			noEmission,
			0.0f,
			sphereSmoothness
		));
	}

	triangleManager.Upload();
	sphereManager.Upload();
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
}
