#version 330 core
layout(location = 0) in vec2 vertexPosition; // Particle vertex (for a point or quad)

uniform vec2 particlePosition;

void main() {
    gl_Position = vec4(particlePosition, 0.0, 1.0); // Use particle position
    gl_PointSize = 5.0; // Size of the particle
}
