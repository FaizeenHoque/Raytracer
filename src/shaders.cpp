#include "headers/shaders.h"

#include <iostream>

#include "glad/glad.h"

std::string getFileContents(const char* filename) {
    std::ifstream in(filename, std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return contents;
    }
    std::cerr << "Failed to open shader file: " << filename << std::endl;
    throw(errno);
}

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
	std::string vertexCode = getFileContents(vertexPath);
	std::string fragmentCode = getFileContents(fragmentPath);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	char log[1024];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 1024, NULL, log);
		std::cerr << "=== VERTEX SHADER COMPILE FAILED (" << vertexPath << ") ===\n" << log << std::endl;
	} else {
		std::cout << "Vertex shader compiled OK: " << vertexPath << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 1024, NULL, log);
		std::cerr << "=== FRAGMENT SHADER COMPILE FAILED (" << fragmentPath << ") ===\n" << log << std::endl;
	} else {
		std::cout << "Fragment shader compiled OK: " << fragmentPath << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 1024, NULL, log);
		std::cerr << "=== SHADER PROGRAM LINK FAILED ===\n" << log << std::endl;
	} else {
		std::cout << "Shader program linked OK." << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Activate() {
    glUseProgram(ID);
}

void Shader::Delete() {
    glDeleteProgram(ID);
}
