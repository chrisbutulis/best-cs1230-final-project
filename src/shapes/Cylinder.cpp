#include "Cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}


void Cylinder::makeTile(
    glm::vec3 topRight,
    glm::vec3 bottomLeft,
    glm::vec3 bottomRight) {
    glm::vec3 normal1 = glm::normalize(glm::cross(topRight - bottomLeft,topRight - bottomRight));
    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,normal1);
    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,normal1);
    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,normal1);
}

void Cylinder::makeTileCuadrilateral(glm::vec3 topLeft,
                                     glm::vec3 topRight,
                                     glm::vec3 bottomLeft,
                                     glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 normal1 = glm::normalize(glm::cross(topLeft - bottomLeft,topLeft - bottomRight));
    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,normal1);
    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,normal1);
    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,normal1);
    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,normal1);
    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,normal1);
    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,normal1);
}

void Cylinder::makeTileSide(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,glm::normalize(glm::vec3{topLeft.x,0.0f,topLeft.z}));
    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,glm::normalize(glm::vec3{bottomLeft.x,0.0f,bottomLeft.z}));
    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,glm::normalize(glm::vec3{topRight.x,0.0f,topRight.z}));
    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,glm::normalize(glm::vec3{topRight.x,0.0f,topRight.z}));
    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,glm::normalize(glm::vec3{bottomLeft.x,0.0f,bottomLeft.z}));
    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,glm::normalize(glm::vec3{bottomRight.x,0.0f,bottomRight.z}));
}

void Cylinder::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!
    float unitRadius = 0.5f/m_param1;

    glm::vec3 leftBase = {0,-0.5f,0};
    glm::vec3 rightBase = {0,-0.5f,0};
    leftBase = glm::vec3(
        unitRadius * glm::cos(nextTheta),
        0.5f,
        unitRadius * glm::sin(nextTheta)
        );

    rightBase = glm::vec3(
        unitRadius * glm::cos(currentTheta),
        0.5f,
        unitRadius* glm::sin(currentTheta)
        );
    makeTile({0,-0.5f,0}, {rightBase.x,-0.5f,rightBase.z},{leftBase.x,-0.5f,leftBase.z});

    makeTile({0,0.5f,0}, leftBase, rightBase);

    for (int i = 0; i < m_param1; i++) {

        glm::vec3 TopLeft = glm::vec3(
            0.5 * glm::cos(nextTheta),
            -0.5f+2*(i+1)*unitRadius,
            0.5 * glm::sin(nextTheta)
            );

        glm::vec3 TopRight = glm::vec3(
            0.5 * glm::cos(currentTheta),
            -0.5f+2*(i+1)*unitRadius,
            0.5 * glm::sin(currentTheta)
            );

        glm::vec3 BotLeft = glm::vec3(
            0.5 * glm::cos(nextTheta),
            -0.5f+2*(i)*unitRadius,
            0.5 * glm::sin(nextTheta)
            );

        glm::vec3 BotRight = glm::vec3(
            0.5 * glm::cos(currentTheta),
            -0.5f+2*(i)*unitRadius,
            0.5 * glm::sin(currentTheta)
            );

        makeTileSide(TopLeft,TopRight, BotLeft, BotRight);

        glm::vec3 bottomLeft = glm::vec3(
            (i+1)*unitRadius * glm::cos(nextTheta),
            0.5f,
            (i+1)*unitRadius * glm::sin(nextTheta)
            );

        glm::vec3 bottomRight = glm::vec3(
            (i+1)*unitRadius * glm::cos(currentTheta),
            0.5f,
            (i+1)*unitRadius * glm::sin(currentTheta)
            );

        // Create the tile for this portion of the wedge
        makeTileCuadrilateral(leftBase,rightBase, bottomLeft, bottomRight);
        makeTileCuadrilateral({rightBase.x,-0.5f,rightBase.z},{leftBase.x,-0.5f,leftBase.z}, {bottomRight.x,-0.5f,bottomRight.z},{bottomLeft.x,-0.5f,bottomLeft.z});
        leftBase = bottomLeft;
        rightBase = bottomRight;
    }
}


void Cylinder::setVertexData() {
    // TODO for Project 5: Lights, Camera
    float thetaStep = glm::radians(360.f /glm::max(m_param2,2));
    for (int i =0; i<    glm::max(m_param2,2); i++){
        makeWedge(i* thetaStep, (i+1) * thetaStep);
    }
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
