#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "fishingline.h"
#include "utils/scenedata.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class ParticleGenerator {
    struct Particle {
        glm::vec3 Position, Velocity;
        glm::vec4 Color;
        float     Life;

        Particle()
          : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
    };
public:
    ParticleGenerator();
    ParticleGenerator(const glm::vec3& position, size_t numParticles);
    void updateParticles(float deltaTime);
    void drawParticles(GLuint m_fullscreen_vao, GLuint m_particle_shader);
    void initParticles();


private:
    //for particles
    std::vector<Particle> particles;
    unsigned int nr_particles = 100;
    glm::vec3 generatorPosition;
};
