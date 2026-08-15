#include "../headers/Triangle.h"

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
	float emissionStrength,
	float smoothness,
	float specularProbability,
	glm::vec3 specularColor
)
	: posA(posA),
	  posB(posB),
	  posC(posC),
	  normalA(normalA),
	  normalB(normalB),
	  normalC(normalC),
	  color(color),
	  emissionColor(emissionColor),
	  emissionStrength(emissionStrength),
      smoothness(smoothness),
	  specularProbability(specularProbability),
      specularColor(specularColor){
}

TriangleManager::TriangleManager(Shader &shader) {
	glGenBuffers(1, &triangleUBO);
	BindToShader(shader);
	VerifyLayout(shader);
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
			MaterialGPU{
				triangle.color,             0.0f,
				triangle.emissionColor,
				triangle.emissionStrength,
				triangle.smoothness,
				triangle.specularProbability,
				0.0f,
				0.0f,
				triangle.specularColor
			}
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

std::size_t TriangleManager::Count() {
	return triangles.size();
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

void TriangleManager::VerifyLayout(Shader& shader) {
	GLuint blockIndex = glGetUniformBlockIndex(shader.ID, "TriangleBuffer");
	if (blockIndex == GL_INVALID_INDEX) return;

	GLint blockSize;
	glGetActiveUniformBlockiv(shader.ID, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

	// blockSize / 128 (array length in the shader) should equal sizeof(TriangleGPU)
	GLint perElement = blockSize / 128;
	if (perElement != sizeof(TriangleGPU)) {
		std::cerr << "TriangleGPU size mismatch! GPU expects "
				   << perElement << " bytes, C++ struct is "
				   << sizeof(TriangleGPU) << " bytes" << std::endl;
	} else {
		std::cout << "TriangleGPU layout OK (" << sizeof(TriangleGPU) << " bytes)" << std::endl;
	}
}
