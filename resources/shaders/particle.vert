#version 330 core
layout(location = 0) in vec2 vertexPosition; // Particle vertex (for a point or quad)

uniform vec3 particlePosition;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

void main() {
    gl_Position = projMatrix * viewMatrix * vec4(particlePosition, 1.0);
    gl_PointSize = 2;
//    gl_Position = vec4(particlePosition, 1.0);
}
