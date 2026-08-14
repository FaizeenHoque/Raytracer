#ifndef RAYTRACER_OBJECTS_H
#define RAYTRACER_OBJECTS_H

#include <glm/glm.hpp>

class Cube {
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 size;
	glm::vec3 color;

	Cube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& size, const glm::vec3& color);
};

#endif