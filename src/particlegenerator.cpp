#include "particlegenerator.h"
#include <iostream>

ParticleGenerator::ParticleGenerator()
    : generatorPosition(4.0f, 0.0f, 4.0f), // Default position at the origin
      nr_particles(1000)                   // Default number of particles
{
    initParticles(); // Initialize the particles
}

void ParticleGenerator::updateParticles(float deltaTime) {
    for (auto &particle : particles) {
        if (particle.Life > 0.0f) {
            // Decrease life
            particle.Life -= deltaTime;
            // Update position based on velocity
            particle.Position += particle.Velocity * deltaTime;
            // Optionally fade out
            particle.Color.a = particle.Life / 5.0f; // Fade alpha over lifetime
        } else {
            // Reset particle
            particle.Position = glm::vec3(generatorPosition.x, generatorPosition.y, generatorPosition.z);
            particle.Velocity = glm::vec3(
                (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f, // Small X variation
                (static_cast<float>(rand()) / RAND_MAX) * 2.0f,        // Strong upward Y velocity
                (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f  // Small Z variation
            );
            particle.Life = static_cast<float>(rand()) / RAND_MAX * 5.0f;
        }
    }
}

void ParticleGenerator::initParticles() {
    particles.resize(nr_particles);
    for (auto &particle : particles) {
        particle.Position = glm::vec3(generatorPosition.x, generatorPosition.y, generatorPosition.z);
        particle.Velocity = glm::vec3(
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f, // Small X variation
            (static_cast<float>(rand()) / RAND_MAX) * 2.0f,        // Strong upward Y velocity
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f  // Small Z variation
        );
        particle.Color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f); // Example: orange color
        particle.Life = static_cast<float>(rand()) / RAND_MAX * 5.0f; // Random life span [0, 5]
    }
}

void ParticleGenerator::drawParticles(GLuint m_fullscreen_vao, GLuint m_particle_shader) {
    glDepthFunc(GL_ALWAYS); // Render particles regardless of depth
    // Render particles
    glUseProgram(m_particle_shader);
    glBindVertexArray(m_fullscreen_vao);
    for (const auto &particle : particles) {
        if (particle.Life > 0.0f) {
            glUniform3f(glGetUniformLocation(m_particle_shader, "particlePosition"), particle.Position.x, particle.Position.y, particle.Position.z);
            glUniform4f(glGetUniformLocation(m_particle_shader, "particleColor"), particle.Color.r, particle.Color.g, particle.Color.b, particle.Color.a);
            glDrawArrays(GL_POINTS, 0, 1); // Assuming particle shader handles size
        }
    }
    glDepthFunc(GL_LESS);   // Reset to default depth testing
}

