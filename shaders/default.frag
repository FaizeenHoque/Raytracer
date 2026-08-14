#version 330 core
out vec4 FragColor;

uniform vec2 screenSize;

void main() {
    float x = gl_FragCoord.y / screenSize.y;
    FragColor = vec4(x, x, x, 1.0f);
}