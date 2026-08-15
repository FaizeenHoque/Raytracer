#include "../headers/triangle.h"

#include <iostream>

Triangle::Triangle(
	glm::vec3 posA,
	glm::vec3 posB,
	glm::vec3 posC,
	glm::vec3 normalA,
	glm::vec3 normalB,
	glm::vec3 normalC,
	glm::vec4 color,
	glm::vec3 emissionColor,
	float emissionStrength
)
	: posA(posA),
	  posB(posB),
	  posC(posC),
	  normalA(normalA),
	  normalB(normalB),
	  normalC(normalC),
	  color(color),
	  emissionColor(emissionColor),
	  emissionStrength(emissionStrength) {
}

TriangleManager::TriangleManager(Shader &shader) {
	glGenBuffers(1, &triangleUBO);
	BindToShader(shader);
}

TriangleManager::~TriangleManager() {
	glDeleteBuffers(1, &triangleUBO);
}

void TriangleManager::AddTriangle(const Triangle &triangle) {
	triangles.push_back(triangle);
}

void TriangleManager::Upload() const {
	std::vector<TriangleGPU> gpuTriangles;
	gpuTriangles.reserve(triangles.size());

	for (const Triangle &triangle : triangles) {
		gpuTriangles.push_back({
			triangle.posA, 0.0f,
			triangle.posB, 0.0f,
			triangle.posC, 0.0f,
			triangle.normalA, 0.0f,
			triangle.normalB, 0.0f,
			triangle.normalC, 0.0f,
			triangle.color, 0.0f,
			triangle.emissionColor, triangle.emissionStrength
		});
	}

	glBindBuffer(GL_UNIFORM_BUFFER, triangleUBO);

	glBufferData(
		GL_UNIFORM_BUFFER,
		gpuTriangles.size() * sizeof(TriangleGPU),
		gpuTriangles.data(),
		GL_STATIC_DRAW
	);

	glBindBufferBase(GL_UNIFORM_BUFFER, TRIANGLE_BUFFER_BINDING, triangleUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void TriangleManager::BindToShader(Shader &shader) {
	shader.Activate();

	GLuint triangleBlockIndex =
		glGetUniformBlockIndex(shader.ID, "TriangleBuffer");

	if (triangleBlockIndex == GL_INVALID_INDEX) {
		std::cerr << "WARNING: TriangleBuffer uniform block not found in shader!" << std::endl;
		return;
	}

	glUniformBlockBinding(
		shader.ID,
		triangleBlockIndex,
		TRIANGLE_BUFFER_BINDING
	);
}