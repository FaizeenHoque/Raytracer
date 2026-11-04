#ifndef RAYTRACER_CUBE_H
#define RAYTRACER_CUBE_H
#include "Triangle.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"


class Cube {
public:
	std::string name = "Cube";
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::vec4 color = glm::vec4(1.0f);
	glm::vec3 emissionColor = glm::vec3(0.0f);
	float emissionStrength = 0.0f;
	float smoothness = 0.0f;
	float specularProbability = 0.0f;
	glm::vec3 specularColor = glm::vec3(1.0f);

	void Build(TriangleManager& triangleManager) const;
};

#endif //RAYTRACER_CUBE_H
