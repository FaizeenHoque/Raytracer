#ifndef RAYTRACER_QUAD_H
#define RAYTRACER_QUAD_H
#include "Triangle.h"

class Quad {
public:
	std::string name = "Default Quad";
	 glm::vec3 scale;
	 glm::vec3 position;
	 glm::vec3 rotation;
	 glm::vec4 color;
	 glm::vec3 emissionColor = glm::vec3(0.0f);
	 glm::vec3 specularColor = glm::vec3(1.0f);
	 float emissionStrength = 0.0f;
	 float smoothness = 0.0f;
	 float specularProbability = 0.0f;

	void Build(TriangleManager& triangleManager) const;
};


#endif //RAYTRACER_QUAD_H
