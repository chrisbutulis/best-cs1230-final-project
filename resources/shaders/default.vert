#version 330 core

layout(location = 0) in vec3 position3d;
layout(location = 1) in vec3 normals;

out vec3 worldPos;
out vec3 worldNormal;
uniform mat4 modelMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

void main() {
    worldPos = vec3(modelMatrix * vec4(position3d, 1.0));
    worldNormal = normalize(transpose(inverse(mat3(modelMatrix))) * normals);
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position3d, 1.0);
}
