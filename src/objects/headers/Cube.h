#ifndef RAYTRACER_CUBE_H
#define RAYTRACER_CUBE_H
#include "Triangle.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"


class Cube {
public:
	Cube(TriangleManager& triangle_manager,
		 glm::vec3 scale,
		 glm::vec3 position,
		 glm::vec3 rotation,
		 glm::vec4 color,
		 float emissionStrenght = 0.0f,
		 glm::vec3 emissionColor = glm::vec3(0.0f),
		 float smoothness = 0.0f,
		 float specularProbability = 0.0f,
		 glm::vec3 specularColor = glm::vec3(1.0f));
};


#endif //RAYTRACER_CUBE_H
