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

	view = glm::lookAt(position, target, worldUp);
}

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

	const GLuint camPosLoc = glGetUniformLocation(shaderProgram.ID, "worldSpaceCameraPos");
	glUniform3fv(camPosLoc, 1, glm::value_ptr(position));

	std::cout << "viewParamsLoc=" << viewParamsLoc << " camMatLoc=" << camMatLoc << " camPosLoc=" << camPosLoc << std::endl;
}

void Camera::Look(GLFWwindow *window, float deltaTime) {
	float amount = rotateSpeed * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		pitch -= amount;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		pitch += amount;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		yaw += amount;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		yaw -= amount;
	}

	pitch = glm::clamp(pitch, -89.0f, 89.0f);

	glm::vec3 newForward;
	newForward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newForward.y = sin(glm::radians(pitch));
	newForward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(newForward);

	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::normalize(glm::cross(forward, worldUp));
	up = glm::cross(right, forward);

	view = glm::lookAt(position, position + forward, worldUp);
}