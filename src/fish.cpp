#include "fish.h"
#include <iostream>

fish::fish(float length):length(length),ctm(glm::mat4(1.0f)) {
    ctm = glm::scale(ctm, glm::vec3(length, length, length));
    basePosition = glm::vec3{3,2,3};
    speed = 1.f; // Random speed
    resetDirectionCooldown();
}

void fish::setBasePosition(glm::vec3 pos) {
    basePosition = pos;

    ctm = glm::mat4(1.0f);
    ctm = glm::translate(ctm, basePosition);
    ctm = glm::scale(ctm, glm::vec3(length, length, length));
}
void fish::changeColor(){
    cAmbient = glm::vec4(1.f,0.f,0.f,0.f);
}

void fish::setRotation(glm::vec3 axis, float angle) {
    ctm = glm::mat4(1.0f);
    ctm = glm::translate(ctm, basePosition);
    ctm = glm::rotate(ctm, angle, axis);
    ctm = glm::scale(ctm, glm::vec3(length, length, length));

    glm::vec4 localForward = glm::vec4(0, 0, 1, 0);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
    look = glm::normalize(glm::vec3(rotationMatrix * localForward));
}

// Resets the cooldown timer to a random interval
void fish::resetDirectionCooldown() {
    changeDirectionCooldown = glm::linearRand(2.0f, 5.0f); // Random time
}

 float timeSinceLastWander = 0.0f; // Time elapsed since the last wander update
 glm::vec3 currentWander = glm::vec3(0.0f); // Store the current random wander vector
float wanderInterval = 5.0f; // Time interval between wander updates
 void fish::update(float deltaTime) {
     timeSinceLastWander += 0.1;

     if (timeSinceLastWander >= wanderInterval) {
         float wanderStrength = 0.05f; // Reduced wander strength for smoother motion
         glm::vec3 newWander = glm::vec3(
             glm::linearRand(-wanderStrength, wanderStrength),
             glm::linearRand(-wanderStrength, wanderStrength),
             glm::linearRand(-wanderStrength, wanderStrength)
             );

         currentWander = glm::mix(currentWander, newWander, 0.1f); // Adjust blend factor for smoothness
         timeSinceLastWander = 0.0f; // Reset the timer
     }

     glm::vec3 desiredDirection = glm::normalize(look + currentWander);
     look = glm::mix(look, desiredDirection, deltaTime * 2.0f);

     float waveAmplitude = 0.05f; // Amplitude of the sinusoidal motion
     float waveFrequency = 1.5f; // Frequency of the sinusoidal motion
     float waveOffset = glm::sin(deltaTime * waveFrequency) * waveAmplitude;

     glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), look));
     basePosition += right * waveOffset;

     moveForward();

     constrainToBounds();

     setRotation(glm::vec3(0, 1, 0), glm::atan(look.x, look.z));
 }


void fish::moveForward() {
    basePosition += look * speed * 0.01f;
    ctm = glm::mat4(1.0f);
    ctm = glm::translate(ctm, basePosition);
    ctm = glm::scale(ctm, glm::vec3(length, length, length));
}

void fish::constrainToBounds() {
    for (int i = 0; i < 3; ++i) {
        if (basePosition[i] > boundingBox || basePosition[i] < -boundingBox) {
            basePosition[i] = glm::clamp(basePosition[i], -boundingBox, boundingBox);
            look[i] = -look[i]; // Reverse direction on that axis
        }
    }

    // Recalculate the transformation matrix after constraining
    ctm = glm::mat4(1.0f);
    ctm = glm::translate(ctm, basePosition);
    ctm = glm::scale(ctm, glm::vec3(length, length, length));
}

