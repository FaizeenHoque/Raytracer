#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H
#include "glm/vec3.hpp"

struct MaterialGPU {
	glm::vec3 color;
	float _pad0;
	glm::vec3 emission;
	float emissionStrength;
	float smoothness;
	float _pad1 = 0.0f;
	float _pad2 = 0.0f;
	float _pad3 = 0.0f;
};

#endif //RAYTRACER_MATERIAL_H
