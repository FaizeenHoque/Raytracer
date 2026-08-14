#include <cmath>

#include "headers/camera.h"

Camera::Camera(Shader shader, float fov, float aspect, float nearPlane, float farPlane)
	: shaderProgram(shader), fov(fov), aspect(aspect), nearPlane(nearPlane), farPlane(farPlane) {
	shaderProgram.Activate();

	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	forward = glm::normalize(target - position);
	right = glm::normalize(glm::cross(forward, worldUp));
	up = glm::cross(right, forward);
}

// void Camera::RayTest() {
// 	float planeHeight = nearPlane * std::tan(glm::radians(fov) * 0.5f) * 2.0f;
// 	float planeWidth = planeHeight * aspect;
//
// 	glm::vec3 bottomLeftLocal = glm::vec3(-planeWidth / 2.0f, -planeHeight / 2.0f, nearPlane);
//
// 	for (int x = 0; x < debugPointCount.x; ++x) {
// 		for (int y = 0; y < debugPointCount.y; ++y) {
// 			float tx = x / (debugPointCount.x - 1.0f);
// 			float ty = y / (debugPointCount.y - 1.0f);
//
// 			glm::vec3 pointLocal = bottomLeftLocal + glm::vec3(planeWidth * tx, planeHeight * ty, 0.0f);
//
// 			glm::vec3 point = position + right * pointLocal.x + up * pointLocal.y + forward * pointLocal.z;
// 			glm::vec3 dir = glm::normalize(point - position);
// 		}
// 	}
// }

void Camera::OnRenderImage() {
	this->UpdateCameraParams();
}

void Camera::UpdateCameraParams() {
	float planeHeight = nearPlane * std::tan(glm::radians(fov) * 0.5f) * 2.0f;
	float planeWidth = planeHeight * aspect;
	shaderProgram.Activate();

	const GLuint viewParamsLoc = glGetUniformLocation(shaderProgram.ID, "viewParams");
	glUniform3fv(viewParamsLoc, 1, glm::value_ptr(glm::vec3(planeWidth, planeHeight, nearPlane)));

	glm::mat4 camLocalToWorld = glm::inverse(view);
	const GLuint camMatLoc = glGetUniformLocation(shaderProgram.ID, "camLocalToWorldMatrix");
	glUniformMatrix4fv(camMatLoc, 1, GL_FALSE, glm::value_ptr(camLocalToWorld));
}
