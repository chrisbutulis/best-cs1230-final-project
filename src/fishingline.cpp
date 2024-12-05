#include "FishingLine.h"

FishingLine::FishingLine(int segments, float radius, float length)
    : m_segments(segments), m_radius(radius), m_segmentLength(length/segments), m_segmentsData(segments) {}

glm::vec3 FishingLine::getVectorToThrow(glm::vec3 target){
    return glm::normalize(target-getlastElement());
};

glm::vec3 FishingLine::getlastElement(){
    Segment lastSegment = m_segmentsData[0];
    return glm::vec3(glm::vec4(lastSegment.start,0));
}

void FishingLine::setLine(const glm::vec3& start, const glm::vec3& end, bool bindStart, float acceleration) {
    glm::vec3 nextPeiceToReach = end; // Start from the start position, not the end
    // Loop through each segment and update the start and end positions
    if (!bindStart){
        for (int i = 0; i < m_segments; ++i) {
            // Calculate the direction for this segment

            glm::vec3 unitDir = glm::normalize(nextPeiceToReach-m_segmentsData[i].end + glm::vec3{0,0.05,0} + m_segmentsData[i].velocity);

            // m_segmentsData[i].velocity = unitDir*acceleration*0.05f;

            // Calculate the start and end of this segment
            glm::vec3 segmentStart = nextPeiceToReach;
            glm::vec3 segmentEnd = nextPeiceToReach - m_segmentLength * unitDir;

            // Store the start and end positions along with the transformation matrix in the Segment struct
            m_segmentsData[i].start = segmentStart;
            m_segmentsData[i].end = segmentEnd;

            // Update nextPeiceToReach to be the end of the current segment
            nextPeiceToReach = segmentEnd;

            // Calculate the midpoint for rotation/scale
            glm::vec3 midpoint = (segmentStart + segmentEnd) * 0.5f;

            // Determine the rotation needed to align with the direction of the segment
            glm::vec3 cylinderAxis(0.0f, 1.0f, 0.0f); // Assuming the cylinder is aligned with the Y-axis
            glm::mat4 rotation = glm::mat4(1.0f);
            if (glm::dot(unitDir, cylinderAxis) < 1.0f - glm::epsilon<float>()) {
                glm::vec3 rotationAxis = glm::cross(cylinderAxis, unitDir);
                float angle = glm::acos(glm::dot(cylinderAxis, unitDir));
                rotation = glm::rotate(glm::mat4(1.0f), angle, glm::normalize(rotationAxis));
            }

            // Create translation and scaling matrices
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), midpoint);
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(m_radius, m_segmentLength, m_radius));

            // Store the computed transformation matrix in the Segment
            m_segmentsData[i].ctm = translation * rotation * scale;
        }
    }

    nextPeiceToReach = start; // Start from the start position, not the end
    // Loop through each segment and update the start and end positions
    for (int i = m_segments-1; i >=0; --i) {
        // Calculate the direction for this segment
        glm::vec3 unitDir = glm::normalize(nextPeiceToReach-m_segmentsData[i].start+ glm::vec3{0,0.05,0}+ m_segmentsData[i].velocity);

        // Calculate the start and end of this segment
        glm::vec3 segmentEnd = nextPeiceToReach;
        glm::vec3 segmentStart = nextPeiceToReach - m_segmentLength * unitDir;

        // Store the start and end positions along with the transformation matrix in the Segment struct
        m_segmentsData[i].start = segmentStart;
        m_segmentsData[i].end = segmentEnd;

        // Update nextPeiceToReach to be the end of the current segment
        nextPeiceToReach = segmentStart;

        // Calculate the midpoint for rotation/scale
        glm::vec3 midpoint = (segmentStart + segmentEnd) * 0.5f;

        // Determine the rotation needed to align with the direction of the segment
        glm::vec3 cylinderAxis(0.0f, 1.0f, 0.0f); // Assuming the cylinder is aligned with the Y-axis
        glm::mat4 rotation = glm::mat4(1.0f);
        if (glm::dot(unitDir, cylinderAxis) < 1.0f - glm::epsilon<float>()) {
            glm::vec3 rotationAxis = glm::cross(cylinderAxis, unitDir);
            float angle = glm::acos(glm::dot(cylinderAxis, unitDir));
            rotation = glm::rotate(glm::mat4(1.0f), angle, glm::normalize(rotationAxis));
        }

        // Create translation and scaling matrices
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), midpoint);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(m_radius, m_segmentLength, m_radius));

        // Store the computed transformation matrix in the Segment
        m_segmentsData[i].ctm = translation * rotation * scale;
    }
}



void FishingLine::render(GLuint shader, const SceneGlobalData& globalData, GLuint m_vao, int m_dataSize) {
    for (const auto& segment : m_segmentsData) {
        // Set per-segment uniforms
        glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, &segment.ctm[0][0]);
        glm::vec4 cAmbient = glm::vec4(1.f, 0.2f, 0.2f, 1.0f);   // Arbitrary ambient color
        glm::vec4 cDiffuse = glm::vec4(1.f, 0.8f, 0.8f, 1.0f);   // Arbitrary diffuse color
        glm::vec4 cSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);  // Arbitrary specular color

        glUniform4fv(glGetUniformLocation(shader, "shapeColor"), 1, glm::value_ptr(cAmbient * globalData.ka));
        glUniform4fv(glGetUniformLocation(shader, "shapeDiffuse"), 1, glm::value_ptr(cDiffuse * globalData.kd));
        glUniform4fv(glGetUniformLocation(shader, "shapeSpecular"), 1, glm::value_ptr(cSpecular * globalData.ks));

        glUniform1f(glGetUniformLocation(shader, "shininess"), 0.01);

        // Bind and draw the segment
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, m_dataSize / 6);
        glBindVertexArray(0);
    }
}
