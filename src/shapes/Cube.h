#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class Cube
{
public:
    void updateParams(int param1);
    std::vector<float> generateShape() { return m_vertexData;
    GLuint vbo, vao;
}

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);

    std::vector<float> m_vertexData;
    int m_param1;
};
