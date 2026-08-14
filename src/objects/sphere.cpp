#include "../headers/sphere.h"

#include <iostream>

Sphere::Sphere(
	const glm::vec3& position,
	float radius,
	const glm::vec4& color
)
	: position(position),
	  radius(radius),
	  color(color)
{
}

SphereManager::SphereManager(Shader& shader) {
	glGenBuffers(1, &sphereUBO);
	BindToShader(shader);
}
SphereManager::~SphereManager() {
	glDeleteBuffers(1, &sphereUBO);
}

void SphereManager::AddSphere(const Sphere& sphere) {
	spheres.push_back(sphere);
}

void SphereManager::Upload() const {
	std::vector<SphereGPU> gpuSpheres;
	gpuSpheres.reserve(spheres.size());

	for (const Sphere& sphere : spheres) {
		gpuSpheres.push_back({sphere.position, sphere.radius, sphere.color});
	}

	glBindBuffer(GL_UNIFORM_BUFFER, sphereUBO);

	glBufferData(GL_UNIFORM_BUFFER, gpuSpheres.size() * sizeof(SphereGPU), gpuSpheres.data(), GL_STATIC_DRAW);
	glBindBufferBase( GL_UNIFORM_BUFFER, SPHERE_BUFFER_BINDING, sphereUBO);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SphereManager::BindToShader(Shader& shader)
{
	shader.Activate();
	GLuint sphereBlockIndex = glGetUniformBlockIndex(shader.ID, "SphereBuffer");

	if (sphereBlockIndex == GL_INVALID_INDEX) {
		std::cerr << "WARNING: SphereBuffer uniform block not found in shader!" << std::endl;
		return;
	}

	glUniformBlockBinding(shader.ID, sphereBlockIndex, SPHERE_BUFFER_BINDING);
}

