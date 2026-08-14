#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#include "shaders.h"

class Camera {
public:
	float fov = 45.0f;
	float aspect = 1.0f;
	float nearPlane = 0.01f;
	float farPlane = 100.0f;

	glm::vec3 position = glm::vec3(0.0f, 1.0f, 5.0f);
	glm::vec3 forward{};
	glm::vec3 up{};
	glm::vec3 right{};

	Camera(Shader shaderProgram, float fov, float aspect, float nearPlane, float farPlane);
	// void RayTest();
	void OnRenderImage();
	void UpdateCameraParams();

private:
	Shader shaderProgram;
	glm::vec2 debugPointCount = glm::vec2(30.0f, 30.0f);
	glm::mat4 projection{};
	glm::mat4 view{};
};

#endif //RAYTRACER_CAMERA_H
