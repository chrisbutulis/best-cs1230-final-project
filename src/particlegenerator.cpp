#include "particlegenerator.h"

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
            particle.Position = glm::vec2(0.0f, 0.0f);
            particle.Velocity = glm::vec2(
                (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f,
                (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f
            );
            particle.Life = static_cast<float>(rand()) / RAND_MAX * 5.0f;
        }
    }
}

void ParticleGenerator::initParticles() {
    particles.resize(nr_particles);
    for (auto &particle : particles) {
        particle.Position = glm::vec2(0.0f, 0.0f); // Center of the screen
        particle.Velocity = glm::vec2(
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f, // Random X velocity [-1, 1]
            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f  // Random Y velocity [-1, 1]
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
            glUniform2f(glGetUniformLocation(m_particle_shader, "particlePosition"), particle.Position.x, particle.Position.y);
            glUniform4f(glGetUniformLocation(m_particle_shader, "particleColor"), particle.Color.r, particle.Color.g, particle.Color.b, particle.Color.a);
            glDrawArrays(GL_POINTS, 0, 1); // Assuming particle shader handles size
        }
    }
    glDepthFunc(GL_LESS);   // Reset to default depth testing
}

