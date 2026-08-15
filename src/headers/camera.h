#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

#include "shaders.h"
#include "GLFW/glfw3.h"

class Camera {
public:
	float fov;
	float aspect;
	float nearPlane;
	float farPlane;
	float focusDistance;

	float sensitivity;
	float walkSpeed;
	float yaw;
	float pitch;

	float divergeStrength;
	float defocusStrength;

	glm::vec3 position = glm::vec3(0.0f, 0.0f, -20.0f);
	glm::vec3 forward{};
	glm::vec3 up{};
	glm::vec3 right{};

	Camera(Shader shaderProgram,
		float fov,
		float aspect,
		float nearPlane,
		float farPlane,
		float focusDistance,
		float sensitivity,
		float walkSpeed,
		float yaw,
		float pitch,
		float divergeStrength,
		float defocusStrength);

	void OnRenderImage(int width, int height);
	void UpdateCameraParams(int width, int height);
	bool Look(GLFWwindow* window, float deltaTime);

private:
	Shader shaderProgram;
	glm::vec2 debugPointCount = glm::vec2(30.0f, 30.0f);
	glm::mat4 projection{};
	glm::mat4 view{};


};

#endif
