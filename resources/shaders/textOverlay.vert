#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 UV;

out vec2 uv;
uniform mat4 projection;

void main() {
    uv = UV;
    gl_Position = projection * vec4(position, 1.0);
}
