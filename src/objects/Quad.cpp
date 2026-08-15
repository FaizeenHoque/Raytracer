#include "headers/Quad.h"

#include <glm/gtc/matrix_transform.hpp>

Quad::Quad(TriangleManager& triangle_manager, glm::vec3 scale, glm::vec3 position,
           glm::vec3 rotation, glm::vec4 color, float emissionStrenght,
           glm::vec3 emissionColor, float smoothness, float specularProbability,
           glm::vec3 specularColor)
{
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, scale);

    const glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transform)));
    const auto xf = [&](glm::vec3 point) { return glm::vec3(transform * glm::vec4(point, 1.0f)); };
    const glm::vec3 normal = glm::normalize(normalMatrix * glm::vec3(0.0f, 1.0f, 0.0f));

    const glm::vec3 bottomLeft(-0.5f, 0.0f, -0.5f);
    const glm::vec3 bottomRight(0.5f, 0.0f, -0.5f);
    const glm::vec3 topRight(0.5f, 0.0f, 0.5f);
    const glm::vec3 topLeft(-0.5f, 0.0f, 0.5f);

    triangle_manager.AddTriangle(Triangle(
        xf(bottomLeft), xf(bottomRight), xf(topRight), normal, normal, normal,
        color, emissionColor, emissionStrenght, smoothness, specularProbability, specularColor));
    triangle_manager.AddTriangle(Triangle(
        xf(bottomLeft), xf(topRight), xf(topLeft), normal, normal, normal,
        color, emissionColor, emissionStrenght, smoothness, specularProbability, specularColor));
}
