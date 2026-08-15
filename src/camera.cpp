#include <cmath>

#include "headers/camera.h"

Camera::Camera(Shader shader,
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
               float defocusStrength)
    : shaderProgram(shader), fov(fov), aspect(aspect),
      nearPlane(nearPlane), farPlane(farPlane), focusDistance(focusDistance), sensitivity(sensitivity),
      walkSpeed(walkSpeed), yaw(yaw), pitch(pitch),
      divergeStrength(divergeStrength), defocusStrength(defocusStrength)
{
    shaderProgram.Activate();

    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    forward = glm::normalize(target - position);
    right = glm::normalize(glm::cross(forward, worldUp));
    up = glm::cross(right, forward);

    view = glm::lookAt(position, target, worldUp);
}

void Camera::OnRenderImage(int width, int height)
{
    this->UpdateCameraParams(width, height);
}

void Camera::UpdateCameraParams(int width, int height)
{
    float planeHeight = focusDistance * std::tan(glm::radians(fov) * 0.5f) * 2.0f;
    float planeWidth = planeHeight * aspect;

    shaderProgram.Activate();

    glUniform2f(glGetUniformLocation(shaderProgram.ID, "screenSize"), (float)width, (float)height);
    glUniform3fv(glGetUniformLocation(shaderProgram.ID, "viewParams"), 1, glm::value_ptr(glm::vec3(planeWidth, planeHeight, focusDistance)));
    glm::mat4 camLocalToWorld = glm::inverse(view);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "camLocalToWorldMatrix"), 1, GL_FALSE, glm::value_ptr(camLocalToWorld));
    glUniform3fv(glGetUniformLocation(shaderProgram.ID, "worldSpaceCameraPos"), 1, glm::value_ptr(position));
    glUniform1f(glGetUniformLocation(shaderProgram.ID, "DivergeStrength"), divergeStrength);
    glUniform1f(glGetUniformLocation(shaderProgram.ID, "DefocusStrength"), defocusStrength);
}

bool Camera::Look(GLFWwindow *window, float deltaTime)
{
    float walkSpeed_ = walkSpeed * deltaTime;

    glm::vec3 oldPosition = position;
    float oldYaw = yaw;
    float oldPitch = pitch;

    static double lastMouseX = 0.0;
    static double lastMouseY = 0.0;
    static bool firstMouse = true;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (firstMouse)
    {
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
        glm::vec3(forward.x, 0.0f, forward.z));

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

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        position = glm::vec3(0.0f, 0.0f, -5.0f);
        yaw = -90.0f;
        pitch = 0.0f;
    }

    view = glm::lookAt(
        position,
        position + forward,
        worldUp);

    return position != oldPosition ||
           yaw != oldYaw ||
           pitch != oldPitch;
}
