#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H
#include "glm/vec3.hpp"

struct MaterialGPU {
	glm::vec3 color;
	float _pad0;
	glm::vec3 emission;
	float emissionStrength;
};
#endif //RAYTRACER_MATERIAL_H
