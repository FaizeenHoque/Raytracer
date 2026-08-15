#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H
#include <cstddef>
#include "glm/vec3.hpp"

struct MaterialGPU {
	glm::vec3 color;
	float _pad0;
	glm::vec3 emission;
	float emissionStrength;
	float smoothness;
	float specularProbability;
	float _pad1 = 0.0f;
	float _pad2 = 0.0f;
	glm::vec3 specularColor;
	float _pad3 = 0.0f;
};

static_assert(sizeof(MaterialGPU) == 64, "MaterialGPU must match the std140 material stride");
static_assert(offsetof(MaterialGPU, color) == 0);
static_assert(offsetof(MaterialGPU, emission) == 16);
static_assert(offsetof(MaterialGPU, emissionStrength) == 28);
static_assert(offsetof(MaterialGPU, smoothness) == 32);
static_assert(offsetof(MaterialGPU, specularProbability) == 36);
static_assert(offsetof(MaterialGPU, specularColor) == 48);

#endif //RAYTRACER_MATERIAL_H
