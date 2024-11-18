#ifndef OPENGLHELPER_H
#define OPENGLHELPER_H

#include "settings.h"
#include "shapes/Cone.h"
#include "shapes/Cube.h"
#include "shapes/Sphere.h"
#include "shapes/cylinder.h"
#include "utils/sceneparser.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class OpenGLHelper {
public:
    static void bindVBOVAO(GLuint* vbo, GLuint* vao);
    static glm::mat4 createPerspectiveProjectionMatrix(float heightAngle, float nearPlane, float farPlane, float aspectRatio);
    static void deleteBuffersAndVAOs(GLuint* buffers, GLuint* arrays, int count);
    static void updateShapesAndBuffers(RenderData& renderData,
                                glm::mat4 &m_view,
                                Cylinder& cylinder,
                                Cone& cone,
                                Cube& cube,
                                Sphere& sphere,
                                const Settings& settings);
};

#endif // OPENGLHELPER_H
