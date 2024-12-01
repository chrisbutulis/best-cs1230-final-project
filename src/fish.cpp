#include "fish.h"
#include <iostream>

fish::fish(float length):length(length),ctm(glm::mat4(1.0f)) {
    ctm = glm::scale(ctm, glm::vec3(length, length, length));
    basePosition = glm::vec3{3,2,3};
    speed = 1.f; // Random speed
    resetDirectionCooldown();
}

void fish::setVAOandDataSize(GLuint vao, int dataSize){
    m_vao = vao;
    m_dataSize = dataSize;
}

void fish::setBasePosition(glm::vec3 pos) {
    basePosition = pos;

    ctm = glm::mat4(1.0f);
    ctm = glm::translate(ctm, basePosition);
    ctm = glm::scale(ctm, glm::vec3(length, length, length));
}

void fish::moveForward() {
    basePosition += look*0.01f;
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


void fish::render(GLuint shader, const SceneGlobalData& globalData) {
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, &ctm[0][0]);

    glUniform4fv(glGetUniformLocation(shader, "shapeColor"), 1, glm::value_ptr(cAmbient * globalData.ka));
    glUniform4fv(glGetUniformLocation(shader, "shapeDiffuse"), 1, glm::value_ptr(cDiffuse * globalData.kd));
    glUniform4fv(glGetUniformLocation(shader, "shapeSpecular"), 1, glm::value_ptr(cSpecular * globalData.ks));

    glUniform1f(glGetUniformLocation(shader, "shininess"), 0.01);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_dataSize / 6);
    glBindVertexArray(0);
}
float angleX;
// Updates the fish's movement per frame
void fish::update(float deltaTime) {
    // Decrement cooldown timer
    // changeDirectionCooldown -= deltaTime;

    // // Change direction if cooldown is over
    // if (changeDirectionCooldown <= 0.0f) {
    //     angleX = glm::linearRand(-0.5f, 0.5f); // Random rotation about X-axis
    //     float angleY = glm::linearRand(-0.5f, 0.5f); // Random rotation about Y-axis
    //     setRotation(glm::cross(look,up),angleX);
    //     resetDirectionCooldown();
    // }
    // Update position using the look vector
    moveForward();
    setRotation(up, glm::sin(deltaTime));
    // Ensure the fish stays inside the bounding box
    constrainToBounds();
}


// Resets the cooldown timer to a random interval
void fish::resetDirectionCooldown() {
    changeDirectionCooldown = glm::linearRand(2.0f, 5.0f); // Random time
}

// Ensures the fish stays inside the bounding box
void fish::constrainToBounds() {
    glm::vec3 constrainedPosition = basePosition;
    for (int i = 0; i < 3; ++i) { // Loop through x, y, z components
        if (basePosition[i] > boundingBox) {
            constrainedPosition[i] = boundingBox;
            look[i] = -abs(look[i]); // Bounce back
        }
        if (basePosition[i] < -boundingBox) {
            constrainedPosition[i] = -boundingBox;
            look[i] = abs(look[i]); // Bounce back
        }
    }
    setBasePosition(constrainedPosition); // Use setBasePosition to update position
    look = glm::normalize(look); // Ensure unit vector
}

// Checks if a position is inside the bounding box
bool fish::isInsideBounds(const glm::vec3& pos) const {
    return glm::all(glm::lessThanEqual(glm::abs(pos), glm::vec3(boundingBox)));
}
