#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class Cone
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }
    GLuint vbo, vao = 0;
private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeTileCuadrilateral(glm::vec3 topLeft,
                               glm::vec3 topRight,
                               glm::vec3 bottomLeft,
                               glm::vec3 bottomRight);
    void makeTileSide(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight);
    void makeTileTop(
        glm::vec3 topRight,
        glm::vec3 bottomLeft,
        glm::vec3 bottomRight);
    void makeTile(
        glm::vec3 topRight,
        glm::vec3 bottomLeft,
        glm::vec3 bottomRight);
    void makeWedge(float currentTheta, float nextTheta);
    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};
