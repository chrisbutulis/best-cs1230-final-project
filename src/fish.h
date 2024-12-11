#ifndef FISH_H
#define FISH_H
#include "utils/scenedata.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

class fish
{
public:
    fish(float length);

    void update(float deltaTime);
    void setBasePosition(glm::vec3 pos);
    void setRotation(glm::vec3 axis, float angle);
    void setVAOandDataSize(GLuint vao, int dataSize);
    void changeColor();
    glm::mat4 ctm;
    glm::vec3 look = glm::vec3{1.f,0.f,0.f};
    glm::vec3 up = glm::vec3{0.f,1.f,0.f};
    void moveForward();

    std::vector<float> fishData;
    GLuint vao;
    GLuint vbo;

    glm::vec4 cAmbient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);   // Arbitrary ambient color
    glm::vec4 cDiffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);   // Arbitrary diffuse color
    glm::vec4 cSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);  // Arbitrary specular color
    float shininess = 32.0f;

private:
    int m_dataSize;
    glm::vec3 basePosition;             // Position of the rod's base

    float length;
    float radius;

    float speed;          // Speed of the fish
    float changeDirectionCooldown; // Cooldown timer for changing direction

    // Constants
    const float minSpeed = 0.5f;
    const float maxSpeed = 2.0f;
    const float boundingBox = 10.0f; // Half-dimension of the 20x20x20 box

    // Helper methods
    void changeDirection();            // Changes the look vector randomly
    void resetDirectionCooldown();     // Resets the cooldown timer
    void constrainToBounds();          // Ensures the fish stays in bounds
    bool isInsideBounds(const glm::vec3& pos) const; // Checks if inside bounds
};

#endif // FISH_H
