#include <cmath>

#include "headers/camera.h"


Camera::Camera(Shader shader, float fov, float aspect, float nearPlane, float farPlane, float sensitivity, float walkSpeed, float yaw, float pitch)
	: shaderProgram(shader), fov(fov), aspect(aspect), nearPlane(nearPlane), farPlane(farPlane), sensitivity(sensitivity), walkSpeed(walkSpeed), yaw(yaw), pitch(pitch) {
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
}

bool Camera::Look(GLFWwindow *window, float deltaTime) {
    float walkSpeed_ = walkSpeed * deltaTime;

    glm::vec3 oldPosition = position;
    float oldYaw = yaw;
    float oldPitch = pitch;

    static double lastMouseX = 0.0;
    static double lastMouseY = 0.0;
    static bool firstMouse = true;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (firstMouse) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
    }

    float xOffset = mouseX - lastMouseX;
    float yOffset = lastMouseY - mouseY;

    lastMouseX = mouseX;
    lastMouseY = mouseY;

    yaw -= xOffset * sensitivity;
    pitch -= yOffset * sensitivity;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 newForward;

    newForward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newForward.y = sin(glm::radians(pitch));
    newForward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    forward = glm::normalize(newForward);

    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

    right = glm::normalize(glm::cross(forward, worldUp));
    up = glm::normalize(glm::cross(right, forward));

    glm::vec3 flatForward = glm::normalize(
        glm::vec3(forward.x, 0.0f, forward.z)
    );

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position -= flatForward * walkSpeed_;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position += flatForward * walkSpeed_;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * walkSpeed_;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * walkSpeed_;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position.y += walkSpeed_;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        position.y -= walkSpeed_;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        position = glm::vec3(0.0f, 0.0f, -5.0f);
        yaw = -90.0f;
        pitch = 0.0f;
    }

    view = glm::lookAt(
        position,
        position + forward,
        worldUp
    );

    return position != oldPosition ||
           yaw != oldYaw ||
           pitch != oldPitch;
}