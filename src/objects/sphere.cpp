#include "../headers/sphere.h"

#include <iostream>

Sphere::Sphere(
	const glm::vec3& position,
	float radius,
	const glm::vec4& color,
	const glm::vec3& emissionColor,
	float emissionStrength,
	float smoothness,
	float specularProbability,
	const glm::vec3& specularColor
)
	: position(position),
	  radius(radius),
	  color(color),
	  emissionColor(emissionColor),
	  emissionStrength(emissionStrength),
	  smoothness(smoothness),
	  specularProbability(specularProbability),
      specularColor(specularColor)
{
}

SphereManager::SphereManager(Shader& shader)
{
	glGenBuffers(1, &sphereUBO);
	BindToShader(shader);
}

SphereManager::~SphereManager()
{
	glDeleteBuffers(1, &sphereUBO);
}

void SphereManager::AddSphere(const Sphere& sphere)
{
	spheres.push_back(sphere);
}

void SphereManager::Upload() const
{
	std::vector<SphereGPU> gpuSpheres;
	gpuSpheres.reserve(spheres.size());

	for (const Sphere& sphere : spheres) {
		gpuSpheres.push_back({
			glm::vec4(sphere.position, sphere.radius),
			MaterialGPU{
				glm::vec3(sphere.color),
				0.0f,
				sphere.emissionColor,
				sphere.emissionStrength,
				sphere.smoothness,
				sphere.specularProbability,
				0.0f,
				0.0f,
				sphere.specularColor
			}
		});
	}
	glBindBuffer(GL_UNIFORM_BUFFER, sphereUBO);

	glBufferData(
		GL_UNIFORM_BUFFER,
		gpuSpheres.size() * sizeof(SphereGPU),
		gpuSpheres.data(),
		GL_STATIC_DRAW
	);

	glBindBufferBase(
		GL_UNIFORM_BUFFER,
		SPHERE_BUFFER_BINDING,
		sphereUBO
	);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

std::size_t SphereManager::Count() {
	return spheres.size();
}

void SphereManager::BindToShader(Shader& shader)
{
	shader.Activate();

	GLuint sphereBlockIndex =
		glGetUniformBlockIndex(shader.ID, "SphereBuffer");

	if (sphereBlockIndex == GL_INVALID_INDEX) {
		std::cerr
			<< "WARNING: SphereBuffer uniform block not found in shader!"
			<< std::endl;

		return;
	}

	glUniformBlockBinding(
		shader.ID,
		sphereBlockIndex,
		SPHERE_BUFFER_BINDING
	);
}
