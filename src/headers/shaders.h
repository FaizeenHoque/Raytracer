#ifndef RAYTRACER_SHADERS_H
#define RAYTRACER_SHADERS_H

#include <fstream>
#include <string>

#include "glad/glad.h"

std::string getFileContents(const char* filename);

class Shader {
public:
    GLuint ID;
    Shader(const char* vertexPath, const char* fragmentPath);

    void Activate();
    void Delete();
};

#endif //RAYTRACER_SHADERS_H
