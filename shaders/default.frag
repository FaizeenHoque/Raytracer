#version 330 core
in vec2 uv;
out vec4 FragColor;

uniform vec3 viewParams;
uniform mat4 camLocalToWorldMatrix;
uniform vec3 worldSpaceCameraPos;

struct Ray {
    vec3 origin;
    vec3 dir;
};

void main() {
    vec3 viewPointLocal = vec3(uv - 0.5, 1.0) * viewParams;
    vec4 viewPoint = camLocalToWorldMatrix * vec4(viewPointLocal, 1.0);

    Ray ray;
    ray.origin = worldSpaceCameraPos;
    ray.dir = normalize(viewPoint.xyz - ray.origin);

    FragColor = vec4(ray.dir, 0.0);
}