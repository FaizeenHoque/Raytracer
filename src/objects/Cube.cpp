#include "headers/Cube.h"

#include <glm/gtc/matrix_transform.hpp>

void Cube::Build(TriangleManager &triangleManager) const {
    // Unit cube corners, centered at origin, -0.5 to 0.5 on each axis
    glm::vec3 minB(-0.5f, -0.5f, -0.5f);
    glm::vec3 maxB(0.5f, 0.5f, 0.5f);

    // Build transform: translate * rotateX * rotateY * rotateZ * scale
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, scale);

    // Normal matrix (inverse-transpose) so non-uniform scale doesn't skew normals
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transform)));

    auto xf = [&](glm::vec3 v) -> glm::vec3 {
        return glm::vec3(transform * glm::vec4(v, 1.0f));
    };
    auto xfNormal = [&](glm::vec3 n) -> glm::vec3 {
        return glm::normalize(normalMatrix * n);
    };

    glm::vec3 n_left(-1.0f, 0.0f, 0.0f);
    glm::vec3 n_right(1.0f, 0.0f, 0.0f);
    glm::vec3 n_front(0.0f, 0.0f, -1.0f);
    glm::vec3 n_back(0.0f, 0.0f, 1.0f);
    glm::vec3 n_top(0.0f, 1.0f, 0.0f);
    glm::vec3 n_bottom(0.0f, -1.0f, 0.0f);

    glm::vec3 nFront = xfNormal(n_front);
    glm::vec3 nBack = xfNormal(n_back);
    glm::vec3 nLeft = xfNormal(n_left);
    glm::vec3 nRight = xfNormal(n_right);
    glm::vec3 nTop = xfNormal(n_top);
    glm::vec3 nBottom = xfNormal(n_bottom);

    // Front
    triangleManager.AddTriangle(Triangle(
        xf(glm::vec3(minB.x, minB.y, minB.z)), xf(glm::vec3(maxB.x, minB.y, minB.z)), xf(glm::vec3(maxB.x, maxB.y, minB.z)),
        nFront, nFront, nFront, color, emissionColor, emissionStrength, smoothness, specularProbability, specularColor));
    triangleManager.AddTriangle(Triangle(
        xf(glm::vec3(minB.x, minB.y, minB.z)), xf(glm::vec3(maxB.x, maxB.y, minB.z)), xf(glm::vec3(minB.x, maxB.y, minB.z)),
        nFront, nFront, nFront, color, emissionColor, emissionStrength, smoothness, specularProbability, specularColor));

    // Back
    triangleManager.AddTriangle(Triangle(
        xf(glm::vec3(minB.x, minB.y, maxB.z)), xf(glm::vec3(minB.x, maxB.y, maxB.z)), xf(glm::vec3(maxB.x, maxB.y, maxB.z)),
        nBack, nBack, nBack, color, emissionColor, emissionStrength, smoothness, specularProbability, specularColor));
    triangleManager.AddTriangle(Triangle(
        xf(glm::vec3(minB.x, minB.y, maxB.z)), xf(glm::vec3(maxB.x, maxB.y, maxB.z)), xf(glm::vec3(maxB.x, minB.y, maxB.z)),
        nBack, nBack, nBack, color, emissionColor, emissionStrength, smoothness, specularProbability, specularColor));

    // Left
    triangleManager.AddTriangle(Triangle(
        xf(glm::vec3(minB.x, minB.y, minB.z)), xf(glm::vec3(minB.x, maxB.y, minB.z)), xf(glm::vec3(minB.x, maxB.y, maxB.z)),
        nLeft, nLeft, nLeft, color, emissionColor, emissionStrength, smoothness, specularProbability, specularColor));
    triangleManager.AddTriangle(Triangle(
        xf(glm::vec3(minB.x, minB.y, minB.z)), xf(glm::vec3(minB.x, maxB.y, maxB.z)), xf(glm::vec3(minB.x, minB.y, maxB.z)),
        nLeft, nLeft, nLeft, color, emissionColor, emissionStrength, smoothness, specularProbability, specularColor));

    // Right
    triangleManager.AddTriangle(Triangle(
        xf(glm::vec3(maxB.x, minB.y, minB.z)), xf(glm::vec3(maxB.x, minB.y, maxB.z)), xf(glm::vec3(maxB.x, maxB.y, maxB.z)),
        nRight, nRight, nRight, color, emissionColor, emissionStrength, smoothness, specularProbability, specularColor));
    triangleManager.AddTriangle(Triangle(
        xf(glm::vec3(maxB.x, minB.y, minB.z)), xf(glm::vec3(maxB.x, maxB.y, maxB.z)), xf(glm::vec3(maxB.x, maxB.y, minB.z)),
        nRight, nRight, nRight, color, emissionColor, emissionStrength, smoothness, specularProbability, specularColor));

    // Top
    triangleManager.AddTriangle(Triangle(
        xf(glm::vec3(minB.x, maxB.y, minB.z)), xf(glm::vec3(maxB.x, maxB.y, minB.z)), xf(glm::vec3(maxB.x, maxB.y, maxB.z)),
        nTop, nTop, nTop, color, emissionColor, emissionStrength, smoothness, specularProbability, specularColor));
    triangleManager.AddTriangle(Triangle(
        xf(glm::vec3(minB.x, maxB.y, minB.z)), xf(glm::vec3(maxB.x, maxB.y, maxB.z)), xf(glm::vec3(minB.x, maxB.y, maxB.z)),
        nTop, nTop, nTop, color, emissionColor, emissionStrength, smoothness, specularProbability, specularColor));

    // Bottom
    triangleManager.AddTriangle(Triangle(
        xf(glm::vec3(minB.x, minB.y, minB.z)), xf(glm::vec3(maxB.x, minB.y, maxB.z)), xf(glm::vec3(maxB.x, minB.y, minB.z)),
        nBottom, nBottom, nBottom, color, emissionColor, emissionStrength, smoothness, specularProbability, specularColor));
    triangleManager.AddTriangle(Triangle(
        xf(glm::vec3(minB.x, minB.y, minB.z)), xf(glm::vec3(minB.x, minB.y, maxB.z)), xf(glm::vec3(maxB.x, minB.y, maxB.z)),
        nBottom, nBottom, nBottom, color, emissionColor, emissionStrength, smoothness, specularProbability, specularColor));
}
