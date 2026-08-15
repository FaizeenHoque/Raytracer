#include "headers/shaders.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "glad/glad.h"

std::string getFileContents(const char* filename) {
    std::ifstream in(filename, std::ios::binary);

    if (!in) {
        std::cerr << "Failed to open shader file: " << filename << std::endl;
        throw std::runtime_error("Failed to open shader file");
    }

    std::stringstream contents;
    contents << in.rdbuf();

    return contents.str();
}

std::string preprocessShader(const std::string& source, const std::string& basePath) {
    std::stringstream output;
    std::istringstream input(source);

    std::string line;

    while (std::getline(input, line)) {
        std::string include = "#include \"";

        size_t includeStart = line.find(include);

        if (includeStart != std::string::npos) {
            size_t pathStart = includeStart + include.length();
            size_t pathEnd = line.find("\"", pathStart);

            if (pathEnd == std::string::npos) {
                throw std::runtime_error("Malformed #include: " + line);
            }

            std::string includePath = line.substr(
                pathStart,
                pathEnd - pathStart
            );

            std::string fullPath = basePath + includePath;

            std::string includedSource = getFileContents(fullPath.c_str());

            output << preprocessShader(includedSource, basePath);
            output << '\n';
        } else {
            output << line << '\n';
        }
    }

    return output.str();
}

std::string loadShader(const char* filename) {
    std::string source = getFileContents(filename);

    std::string path(filename);
    size_t slash = path.find_last_of("/\\");

    std::string basePath;

    if (slash != std::string::npos) {
        basePath = path.substr(0, slash + 1);
    }

    return preprocessShader(source, basePath);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = loadShader(vertexPath);
    std::string fragmentCode = loadShader(fragmentPath);

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

        std::cerr
            << "=== VERTEX SHADER COMPILE FAILED ("
            << vertexPath
            << ") ===\n"
            << log
            << std::endl;
    } else {
        std::cout
            << "Vertex shader compiled OK: "
            << vertexPath
            << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 1024, NULL, log);

        std::cerr
            << "=== FRAGMENT SHADER COMPILE FAILED ("
            << fragmentPath
            << ") ===\n"
            << log
            << std::endl;
    } else {
        std::cout
            << "Fragment shader compiled OK: "
            << fragmentPath
            << std::endl;
    }

    ID = glCreateProgram();

    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(ID, 1024, NULL, log);

        std::cerr
            << "=== SHADER PROGRAM LINK FAILED ===\n"
            << log
            << std::endl;
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