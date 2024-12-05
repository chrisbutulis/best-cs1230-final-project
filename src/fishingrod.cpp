#include "FishingRod.h"

FishingRod::FishingRod(float length, float radius)
    : length(length), radius(radius), basePosition(glm::vec3(0.0f)), ctm(glm::mat4(1.0f)),m_line(10,0.1,5) {
    ctm = glm::scale(ctm, glm::vec3(radius, length, radius));
}

void FishingRod::update(float deltaTime) {
    // Placeholder for dynamic updates, such as physics or kinematics
}
// glm::vec3 rodOffset = glm::vec3(-0.3f, -0.2f, -0.5f);
glm::vec3 rodOffset = glm::vec3(0,0,0);


void FishingRod::updatePosition(const glm::vec3& cameraPos, const glm::vec3& cameraDirection, const glm::mat4& viewMatrix) {
    // Calculate the base position of the rod near the camera
    glm::vec3 rodBasePosition = cameraPos + cameraDirection * rodOffset.z + glm::vec3(rodOffset.x, rodOffset.y, 0.0f);

    // Rotate rod based on camera direction
    glm::mat4 rotation = glm::lookAt(cameraPos, cameraPos + cameraDirection, glm::vec3(0, 1, 0));

    // Apply translation and rotation
    ctm = glm::translate(glm::mat4(1.0f), rodBasePosition) * rotation;
}

bool FishingRod::collition(glm::vec3 pos) {
    return glm::length(m_line.getlastElement()-pos) < 1.5f;
}

void FishingRod::setBasePosition(glm::vec3 pos) {
    basePosition = pos;

    ctm = glm::mat4(1.0f);
    ctm = glm::translate(ctm, basePosition);
    ctm = glm::scale(ctm, glm::vec3(radius, length, radius));
}

void FishingRod::setRotation(glm::vec3 axis, float angle) {
    ctm = glm::mat4(1.0f);
    ctm = glm::translate(ctm, basePosition);
    ctm = glm::rotate(ctm, angle, axis);
    ctm = glm::scale(ctm, glm::vec3(radius, length, radius));
}

void FishingRod::setVAOandDataSize(GLuint vao, int dataSize){
    m_vao = vao;
    m_dataSize = dataSize;
}

void FishingRod::setLineEnd(const glm::vec3& end) {
    glm::vec3 tipWorldPosition = glm::vec3(ctm * glm::vec4(0.0f, 0.5f, 0.0f, 1.0f));
    // m_line.setLine(tipWorldPosition, end,true, );
}
glm::vec3 vecToThrow;
glm::vec3 vecEndrod;

void FishingRod::drawFishingRodBack(float pressDuration,glm::vec3 right) {
    // Calculate backward rotation based on pressDuration (max 45 degrees)
    float maxAngle = glm::radians(45.0f);
    float rotationAngle = maxAngle * pressDuration;
    setRotation(right, rotationAngle);

    // Update the line position
    glm::vec3 tipWorldPosition = glm::vec3(ctm * glm::vec4(0.0f, 0.5f, 0.0f, 1.0f));
    m_line.setLine(tipWorldPosition, glm::vec3{0, 0, 0}, true, pressDuration);
}

void FishingRod::drawFishingRodForward(float deltaTime, glm::vec3 right,glm::vec3  intersect) {
    if (deltaTime < 5.f) {
        // Smoothly rotate forward based on time
        float forwardAngle = glm::radians(45.0f) * (2.5f - deltaTime) / 2.5f;
        setRotation(right, forwardAngle);
    } else {
        // Line continues forward
        glm::vec3 tipWorldPosition = glm::vec3(ctm * glm::vec4(0.0f, 0.5f, 0.0f, 1.0f));
        m_line.setLine(tipWorldPosition, intersect*deltaTime/30.f + vecEndrod*(1-deltaTime/30.f), false,deltaTime);
        return;
    }

    glm::vec3 tipWorldPosition = glm::vec3(ctm * glm::vec4(0.0f, 0.5f, 0.0f, 1.0f));
    m_line.setLine(tipWorldPosition, glm::vec3{0, 0, 0}, true,deltaTime);
    vecEndrod = m_line.getlastElement();
}
void FishingRod::render(GLuint shader, const SceneGlobalData& globalData) {

    glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, &ctm[0][0]);
    glm::vec4 cAmbient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);   // Arbitrary ambient color
    glm::vec4 cDiffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);   // Arbitrary diffuse color
    glm::vec4 cSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);  // Arbitrary specular color

    glUniform4fv(glGetUniformLocation(shader, "shapeColor"), 1, glm::value_ptr(cAmbient * globalData.ka));
    glUniform4fv(glGetUniformLocation(shader, "shapeDiffuse"), 1, glm::value_ptr(cDiffuse * globalData.kd));
    glUniform4fv(glGetUniformLocation(shader, "shapeSpecular"), 1, glm::value_ptr(cSpecular * globalData.ks));

    glUniform1f(glGetUniformLocation(shader, "shininess"), 0.01);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_dataSize / 6);
    glBindVertexArray(0);
    m_line.render(shader,globalData,m_vao,m_dataSize);

}
