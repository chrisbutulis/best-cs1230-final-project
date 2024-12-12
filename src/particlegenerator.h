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
        float CooldownTime;

        Particle()
          : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f), CooldownTime(0.0f) { }
    };

public:
    enum class GeneratorType {
        CRUNK,
        CRUNK2,
        FIREWORKS,
        FLARE
    };

    ParticleGenerator(GeneratorType type);
    ParticleGenerator(GeneratorType type, glm::vec3 position, int nr_particles);
    void updateParticles(float deltaTime);
    void drawParticles(GLuint m_fullscreen_vao, GLuint m_particle_shader);
    void initParticles();
    void initHelper(Particle &particle);

private:
    //for particles
    std::vector<Particle> particles;
    unsigned int nr_particles = 100;
    glm::vec3 generatorPosition;
    GeneratorType m_type;
};
