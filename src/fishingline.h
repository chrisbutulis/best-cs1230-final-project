#ifndef FISHINGLINE_H
#define FISHINGLINE_H

#include "utils/scenedata.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class FishingLine {
public:
    // Struct to represent each segment with start, end, and the corresponding transformation matrix
    struct Segment {
        glm::vec3 start = {1,1,1};    // Position of the start of the segment
        glm::vec3 end = {0,0,0};      // Position of the end of the segment
        glm::mat4 ctm;      // Transformation matrix for the segment
        glm::vec3 velocity = {0,0,0};
        glm::vec3 acceleration = {0,0,0};
    };

    FishingLine(int segments, float radius, float length);
    void setLine(const glm::vec3& start, const glm::vec3& end, bool bindStart, float acceleration);
    void render(GLuint shader, const SceneGlobalData& globalData, GLuint m_vao, int m_dataSize);
    glm::vec3 getVectorToThrow(glm::vec3 target);
    void computeTransformMatrix(Segment& segment, const glm::vec3& unitDir);
    glm::vec3 getlastElement();
    void teardown(float t);

private:
    int m_segments;                    // Number of segments in the line
    float m_radius;                    // Radius of each cylinder (segment)
    float m_segmentLength;             // Length of each segment
    std::vector<Segment> m_segmentsData; // Vector of segment structs (start, end, ctm)
};

#endif // FISHINGLINE_H
