#include "particlegenerator.h"
#include <iostream>

/**
 * @brief ParticleGenerator::ParticleGenerator
 * To have a particle generate and draw, instantiate a particle generator in realtime, use the following function calls in paintGL:

        glUseProgram(m_particle_shader);
        PaintGLHelper::setupMatrices(m_particle_shader, m_view, renderData.cameraData);
        generator.drawParticles(m_fbo, m_particle_shader);

    And then call generator.updateParticles(deltaTime); in timerEvent

 */

ParticleGenerator::ParticleGenerator(GeneratorType type)
    : generatorPosition(4.0f, 4.f, 4.0f), // Default position at the origin
      nr_particles(1000)                   // Default number of particles
{
    initParticles(); // Initialize the particles
    m_type = type;
}

void ParticleGenerator::updateParticles(float deltaTime) {
    for (auto &particle : particles) {
        if (particle.Life > 0.0f) {
            particle.Life -= deltaTime;

            particle.Position += particle.Velocity * deltaTime;
            particle.Velocity *= 0.95f; // Decelerate particles (if desired)

            // If the particle is dead, start the cooldown
            float coolDown = 1.f;
            if (particle.CooldownTime < coolDown) {
                particle.CooldownTime += deltaTime;

            }
            else {
                initHelper(particle); // Reinitialize particle
            }
        }
    }
}

void ParticleGenerator::initHelper(Particle &particle) {
        particle.Position = glm::vec3(generatorPosition.x, generatorPosition.y, generatorPosition.z);
        float theta = glm::radians(static_cast<float>(rand() % 360));
        float phi = glm::radians(static_cast<float>(rand() % 180 - 90));
        float x = cos(phi) * cos(theta);
        float y = cos(phi) * sin(theta);
        float z = sin(phi);

        float speed = static_cast<float>(rand() % 100) / 10.0f + 5.0f; // Random speed between 5 and 15
        particle.Velocity = glm::vec3(x, y, z) * speed;
        particle.Color = glm::vec4(1.f, 0.7f, 0.0f, 1.0f);
        particle.Life = 2;
        particle.CooldownTime = 0.0f;
//    if(m_type == GeneratorType::FLARE){
//        particle.Position = glm::vec3(generatorPosition.x, generatorPosition.y, generatorPosition.z);

//        // FLARE particles start with a small random velocity
//        particle.Velocity = glm::vec3(
//            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f, // Small X variation
//            (static_cast<float>(rand()) / RAND_MAX) * 2.0f,        // Strong upward Y velocity
//            (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.5f  // Small Z variation
//        );

//        particle.Color = glm::vec4(1.f, 0.7f, 0.0f, 1.0f);
//        particle.Life = static_cast<float>(rand()) / RAND_MAX * 5.0f;
//        particle.CooldownTime = 0.0f;
//    }
}

void ParticleGenerator::initParticles() {
    particles.resize(nr_particles);
    for (auto &particle : particles) {
        initHelper(particle); // Initialize each particle with the updated function
    }
}

void ParticleGenerator::drawParticles(GLuint m_fullscreen_vao, GLuint m_particle_shader) {
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render particles
    glUseProgram(m_particle_shader);
    glBindVertexArray(m_fullscreen_vao);
    for (const auto &particle : particles) {
        if (particle.Life > 0.0f) {
            glUniform1f(glGetUniformLocation(m_particle_shader, "life"), particle.Life);
            glUniform3f(glGetUniformLocation(m_particle_shader, "particlePosition"), particle.Position.x, particle.Position.y, particle.Position.z);
            glUniform4f(glGetUniformLocation(m_particle_shader, "particleColor"), particle.Color.r, particle.Color.g, particle.Color.b, particle.Color.a);
            glDrawArrays(GL_POINTS, 0, 1); // Assuming particle shader handles size
        }
    }
}

