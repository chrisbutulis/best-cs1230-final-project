#include "Cone.h"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}


void Cone::makeTile(
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

void Cone::makeTileTop(
    glm::vec3 topRight,
    glm::vec3 bottomLeft,
    glm::vec3 bottomRight) {
    float slopeY = 0.5f; // r/h for a unit cone
    glm::vec3 normalTopRight = glm::normalize(glm::vec3(topRight.x, 0.25 - 0.5 * topRight.y, topRight.z));
    glm::vec3 normalBottomLeft = glm::normalize(glm::vec3(2*bottomLeft.x, 0.25 - 0.5 * bottomLeft.y, 2*bottomLeft.z));
    glm::vec3 normalBottomRight = glm::normalize(glm::vec3(2*bottomRight.x, 0.25 - 0.5 * bottomRight.y, 2*bottomRight.z));

    insertVec3(m_vertexData,glm::vec3(0.f,0.5f,0.f));
    insertVec3(m_vertexData,normalTopRight);
    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,normalBottomLeft);
    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,normalBottomRight);
}

void Cone::makeTileCuadrilateral(glm::vec3 topLeft,
                                 glm::vec3 topRight,
                                 glm::vec3 bottomLeft,
                                 glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 normal1 = glm::normalize(glm::vec3{0,-1,0});
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal1);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal1);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal1);

    // Insert vertices and normals for the second triangle (topRight, bottomLeft, bottomRight)

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal1);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal1);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal1);
}

void Cone::makeTileSide(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight) {
    // Calculate normals for each vertex based on the true slope of the cone
    // float slopeY = 0.25 - 0.5 * y; // r/h for a unit cone

    glm::vec3 normalTopLeft = glm::normalize(glm::vec3(2*topLeft.x, 0.25 - 0.5 * topLeft.y, 2*topLeft.z));
    glm::vec3 normalTopRight = glm::normalize(glm::vec3(2*topRight.x, 0.25 - 0.5 * topRight.y, 2*topRight.z));
    glm::vec3 normalBottomLeft = glm::normalize(glm::vec3(2*bottomLeft.x, 0.25 - 0.5 * bottomLeft.y, 2*bottomLeft.z));
    glm::vec3 normalBottomRight = glm::normalize(glm::vec3(2*bottomRight.x, 0.25 - 0.5 * bottomRight.y, 2*bottomRight.z));

    // Insert vertices and normals for the first triangle (topLeft, bottomLeft, topRight)
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalTopRight);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);

    // Insert vertices and normals for the second triangle (topRight, bottomLeft, bottomRight)

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottomLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);
}



void Cone::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!

    float unitRadius = 0.5f/m_param1;

    glm::vec3 leftBase = {0,0,0};
    glm::vec3 rightBase = {0,0,0};
    leftBase = glm::vec3(
        unitRadius * glm::cos(nextTheta),
        -0.5f,
        unitRadius * glm::sin(nextTheta)
        );

    rightBase = glm::vec3(
        unitRadius * glm::cos(currentTheta),
        -0.5f,
        unitRadius* glm::sin(currentTheta)
        );
    makeTileTop({(glm::cos(nextTheta) + glm::cos(currentTheta))*0.5f ,0.5f-2*0.5f,(glm::sin(nextTheta) + glm::sin(currentTheta))*0.5f}, {leftBase.x,0.5f-2*unitRadius,leftBase.z},{rightBase.x,0.5f-2*unitRadius,rightBase.z});

    makeTile({0,-0.5f,0}, rightBase,leftBase);

    for (int i = 1; i < m_param1; i++) {

        glm::vec3 bottomLeft = glm::vec3(
            (i+1)*unitRadius * glm::cos(nextTheta),
            -0.5f,
            (i+1)*unitRadius * glm::sin(nextTheta)
            );

        glm::vec3 bottomRight = glm::vec3(
            (i+1)*unitRadius * glm::cos(currentTheta),
            -0.5f,
            (i+1)*unitRadius * glm::sin(currentTheta)
            );

        // Create the tile for this portion of the wedge
        makeTileCuadrilateral(rightBase,leftBase, bottomRight,bottomLeft);
        makeTileSide({leftBase.x,0.5f-2*(i)*unitRadius,leftBase.z},{rightBase.x,0.5f-2*(i)*unitRadius,rightBase.z},{bottomLeft.x,0.5f-2*(i+1)*unitRadius,bottomLeft.z},{bottomRight.x,0.5f-2*(i+1)*unitRadius,bottomRight.z});
        leftBase = bottomLeft;
        rightBase = bottomRight;
    }
}


void Cone::setVertexData() {
    // TODO for Project 5: Lights, Camera
    float thetaStep = glm::radians(360.f / glm::max(m_param2,2));
    for (int i =0; i<glm::max(m_param2,2); i++){
        makeWedge(i* thetaStep, (i+1) * thetaStep);
    }
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
