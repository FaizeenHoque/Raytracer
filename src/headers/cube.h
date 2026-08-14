//
// Created by fynr1x on 8/14/26.
//

#ifndef RAYTRACER_OBJECTS_H
#define RAYTRACER_OBJECTS_H

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader;

class Cube
{
private:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;


public:
	glm::vec3 rotation;
	glm::vec3 position;

	Cube(const glm::vec3& position, const glm::vec3& rotation);

	void Draw(Shader& shader);

	~Cube();
};

#endif //RAYTRACER_OBJECTS_H
