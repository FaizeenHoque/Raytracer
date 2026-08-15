#ifndef RAYTRACER_TRIANGLE_H
#define RAYTRACER_TRIANGLE_H
#include "../../headers/material.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "../../headers/shaders.h"



struct TriangleGPU {
	glm::vec3 posA;    float _pad0;
	glm::vec3 posB;    float _pad1;
	glm::vec3 posC;    float _pad2;
	glm::vec3 normalA; float _pad3;
	glm::vec3 normalB; float _pad4;
	glm::vec3 normalC; float _pad5;
	MaterialGPU material;
};

static_assert(sizeof(TriangleGPU) == 160, "TriangleGPU must match the std140 triangle stride");
static_assert(offsetof(TriangleGPU, material) == 96);

class Triangle {
public:
	glm::vec3 posA;
	glm::vec3 posB;
	glm::vec3 posC;

	glm::vec3 normalA;
	glm::vec3 normalB;
	glm::vec3 normalC;

	glm::vec4 color;
	glm::vec3 emissionColor;
	float emissionStrength;
	float smoothness;
	float specularProbability;
	glm::vec3 specularColor;

	Triangle(
		glm::vec3 posA,
		glm::vec3 posB,
		glm::vec3 posC,
		glm::vec3 normalA,
		glm::vec3 normalB,
		glm::vec3 normalC,
		glm::vec4 color,
		glm::vec3 emissionColor,
		float emissionStrength,
		float smoothness = 0.0f,
		float specularProbability = 0.0f,
		glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f)
	);
};

class TriangleManager {
public:
	TriangleManager(Shader& shader);
	~TriangleManager();

	void AddTriangle(const Triangle& triangle);
	void Upload() const;

	std::size_t Count();

private:
	void BindToShader(Shader& shader);
	void VerifyLayout(Shader& shader);

	std::vector<Triangle> triangles;

	GLuint triangleUBO = 0;

	static constexpr GLuint TRIANGLE_BUFFER_BINDING = 1;
};


#endif
