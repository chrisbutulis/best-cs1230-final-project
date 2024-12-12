#version 330 core
layout (location = 0) in vec2 position;  // Quad position
layout (location = 1) in vec2 texCoords; // Texture coordinates

out vec2 uv;

uniform mat4 projection; // Orthographic projection matrix

void main() {
    uv = texCoords;
    gl_Position = projection * vec4(position, 0.0, 1.0);
}
