#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include <vector>

#include <glm/glm.hpp>

#include "shaders.h"
#include "glad/glad.h"

struct SphereGPU {
	glm::vec4 positionRadius;
	glm::vec4 color;
	glm::vec4 emission;
};

class Sphere {
public:
	glm::vec3 position;
	float radius;

	glm::vec4 color;

	glm::vec3 emissionColor;
	float emissionStrength;

	Sphere(
		const glm::vec3& position,
		float radius,
		const glm::vec4& color,
		const glm::vec3& emissionColor,
		float emissionStrength
	);
};

class SphereManager {
public:
	SphereManager(Shader& shader);
	~SphereManager();

	void AddSphere(const Sphere& sphere);
	void Upload() const;

private:
	void BindToShader(Shader& shader);

	std::vector<Sphere> spheres;

	GLuint sphereUBO = 0;

	static constexpr GLuint SPHERE_BUFFER_BINDING = 0;
};

#endif // RAYTRACER_SPHERE_H