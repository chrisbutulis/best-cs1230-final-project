#include "openglhelper.h"
#include <iostream>

void OpenGLHelper::bindVBOVAO(GLuint* vbo, GLuint* vao) {
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 0, 0, GL_STATIC_DRAW);

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0); // position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // normal
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

glm::mat4 OpenGLHelper::createPerspectiveProjectionMatrix(float heightAngle, float nearPlane, float farPlane, float aspectRatio) {
    float c = -nearPlane / farPlane;

    glm::mat4 reMap = glm::mat4(0.0f);
    reMap[0][0] = 1.0f;
    reMap[1][1] = 1.0f;
    reMap[2][2] = -2.f;
    reMap[3][3] = 1.0f;
    reMap[3][2] = -1.f;

    glm::mat4 depthRangeMatrix = glm::mat4(0.0f);
    depthRangeMatrix[0][0] = 1.0f;
    depthRangeMatrix[1][1] = 1.0f;
    depthRangeMatrix[2][2] = 1.f / (1.f + c);
    depthRangeMatrix[2][3] = -1.0f;
    depthRangeMatrix[3][2] = -c / (1.f + c);

    glm::mat4 perspectiveDivisionMatrix = glm::mat4(0.0f);
    perspectiveDivisionMatrix[0][0] = 1.0f / (farPlane * aspectRatio * tan(heightAngle / 2));
    perspectiveDivisionMatrix[1][1] = 1.0f / (farPlane * tan(heightAngle / 2));
    perspectiveDivisionMatrix[2][2] = 1.f / farPlane;
    perspectiveDivisionMatrix[3][3] = 1.0f;

    glm::mat4 perspectiveMatrix = reMap * depthRangeMatrix * perspectiveDivisionMatrix;
    return perspectiveMatrix;
}

void OpenGLHelper::deleteBuffersAndVAOs(GLuint* buffers, GLuint* arrays, int count) {
    glDeleteBuffers(count, buffers);
    glDeleteVertexArrays(count, arrays);
}

void OpenGLHelper::updateShapesAndBuffers(RenderData& renderData,
                            glm::mat4 &m_view,
                            Cylinder& cylinder,
                            Cone& cone,
                            Cube& cube,
                            Sphere& sphere,
                            const Settings& settings) {
    // Update shape parameters and buffer data
    if (!renderData.shapes.empty()) {
        cylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);
        glBindBuffer(GL_ARRAY_BUFFER, cylinder.vbo);
        auto cylinderData = cylinder.generateShape();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cylinderData.size(), cylinderData.data(), GL_STATIC_DRAW);

        cone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
        glBindBuffer(GL_ARRAY_BUFFER, cone.vbo);
        auto coneData = cone.generateShape();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * coneData.size(), coneData.data(), GL_STATIC_DRAW);

        cube.updateParams(settings.shapeParameter1);
        glBindBuffer(GL_ARRAY_BUFFER, cube.vbo);
        auto cubeData = cube.generateShape();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cubeData.size(), cubeData.data(), GL_STATIC_DRAW);

        sphere.updateParams(settings.shapeParameter1, settings.shapeParameter2);
        glBindBuffer(GL_ARRAY_BUFFER, sphere.vbo);
        auto sphereData = sphere.generateShape();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sphereData.size(), sphereData.data(), GL_STATIC_DRAW);
    }

    // Set up the perspective projection matrix
    m_view = OpenGLHelper::createPerspectiveProjectionMatrix(
        renderData.cameraData.heightAngle,
        settings.nearPlane,
        settings.farPlane,
        settings.aspectRatio
        );

    // Calculate data sizes for shapes
    for (auto& shape : renderData.shapes) {
        switch (shape.primitive.type) {
        case PrimitiveType::PRIMITIVE_CYLINDER:
            shape.dataSize = cylinder.generateShape().size();
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            shape.dataSize = cone.generateShape().size();
            break;
        case PrimitiveType::PRIMITIVE_CUBE:
            shape.dataSize = cube.generateShape().size();
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            shape.dataSize = sphere.generateShape().size();
            break;
        default:
            break;
        }
    }
}

