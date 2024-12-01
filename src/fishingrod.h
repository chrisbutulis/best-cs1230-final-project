#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "FishingLine.h"
#include "utils/scenedata.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


class FishingRod {
public:
    FishingRod(float length, float radius);

    void update(float deltaTime);       // Update the rod (future physics/kinematics)
    void render(GLuint shader, const SceneGlobalData& globalData);                // Render the rod
    void setBasePosition(glm::vec3 pos); // Set base position
    void setRotation(glm::vec3 axis, float angle); // Rotate rod
    void setVAOandDataSize(GLuint vao, int dataSize);
    void setLineEnd(const glm::vec3& end);
    void drawFishingRodForward(float deltaTime, glm::vec3 right,glm::vec3 intersect);
    void drawFishingRodBack(float pressDuration, glm::vec3 right);
    void updatePosition(const glm::vec3& cameraPos, const glm::vec3& cameraDirection, const glm::mat4& viewMatrix);
    bool collition(glm::vec3 pos);
private:
    GLuint m_vao;
    int m_dataSize;
    glm::mat4 ctm;                      // Cumulative transformation matrix
    glm::vec3 basePosition;             // Position of the rod's base
    float length;                       // Length of the rod
    float radius;                       // Radius of the rod
    FishingLine m_line;
};
