#include "Cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 normal1 = glm::normalize(glm::cross(topLeft - bottomLeft,topLeft - bottomRight));
    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,normal1);
    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,normal1);
    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,normal1);
    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,normal1);
    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,normal1);
    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,normal1);
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube
    glm::vec3 diffhorizontal = -(topLeft - topRight)* (1.0f / m_param1);
    glm::vec3 diffvertical = -(topLeft - bottomLeft)* (1.0f / m_param1);
    for (int iInt = 0; iInt<m_param1;iInt++){
        for (int jInt =0; jInt<m_param1;jInt++){
            float i = static_cast<float>(iInt);
            float j = static_cast<float>(jInt);
            makeTile(topLeft+i*diffhorizontal+j*diffvertical, topLeft+diffhorizontal+i*diffhorizontal+j*diffvertical,
                     topLeft+diffvertical+i*diffhorizontal+j*diffvertical, topLeft+diffhorizontal+diffvertical+i*diffhorizontal+j*diffvertical);
        }
    }
}

void Cube::setVertexData() {
    // Uncomment these lines for Task 2, then comment them out for Task 3:

    // makeTile(glm::vec3(-0.5f,  0.5f, 0.5f),
    //          glm::vec3( 0.5f,  0.5f, 0.5f),
    //          glm::vec3(-0.5f, -0.5f, 0.5f),
    //          glm::vec3( 0.5f, -0.5f, 0.5f));

    // Uncomment these lines for Task 3:

    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));

    // Task 4: Use the makeFace() function to make all 6 sides of the cube
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));

    makeFace(glm::vec3(0.5f,-0.5f,  0.5f),
             glm::vec3(0.5f, 0.5f,  0.5f),
             glm::vec3(0.5f,-0.5f, -0.5f),
             glm::vec3(0.5f,0.5f, -0.5f));

    makeFace(
        glm::vec3(-0.5f, 0.5f,  0.5f),
        glm::vec3(-0.5f,-0.5f,  0.5f),
        glm::vec3(-0.5f,0.5f, -0.5f),
        glm::vec3(-0.5f,-0.5f, -0.5f));

    makeFace(glm::vec3(-0.5f,-0.5f,  0.5f),
             glm::vec3(-0.5f, 0.5f,  0.5f),
             glm::vec3(-0.5f,-0.5f, -0.5f),
             glm::vec3(-0.5f,0.5f, -0.5f));

    makeFace(
        glm::vec3( 0.5f,0.5f,  0.5f),
        glm::vec3(-0.5f,0.5f,  0.5f),
        glm::vec3(0.5f,0.5f, -0.5f),
        glm::vec3(-0.5f,0.5f, -0.5f));

    makeFace(glm::vec3(-0.5f,-0.5f,  0.5f),
             glm::vec3( 0.5f,-0.5f,  0.5f),
             glm::vec3(-0.5f,-0.5f, -0.5f),
             glm::vec3(0.5f,-0.5f, -0.5f));
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
