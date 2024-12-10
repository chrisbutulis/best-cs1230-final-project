#include "paintglhelper.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>



glm::mat4 getTranslationMatrix(float dx, float dy, float dz) {
    return glm::mat4(
        1.f, 0.f, 0.f, 0.f,  // Column 1
        0.f, 1.f, 0.f, 0.f,  // Column 2
        0.f, 0.f, 1.f, 0.f,  // Column 3
        dx, dy, dz, 1.f      // Column 4 (translation components)
        );
}



glm::mat4 PaintGLHelper::calculateViewMatrix(const SceneCameraData &camera) {
    // Camera position, target, and up vector
    glm::vec3 pos = glm::vec3(camera.pos);
    glm::vec3 look = glm::vec3(camera.look);
    glm::vec3 up = glm::vec3(camera.up);

    glm::mat4 translation = getTranslationMatrix(-pos.x, -pos.y, -pos.z);

    glm::vec3 w = -glm::normalize(look); // Forward vector (camera's view direction)
    glm::vec3 v = glm::normalize(up - glm::dot(up,w) * w); // Right vector
    glm::vec3 u = glm::cross(v, w); // Up vector

    glm::mat4 rotate = glm::mat4(
        u.x, v.x, w.x, 0.0f,
        u.y, v.y, w.y, 0.0f,
        u.z, v.z, w.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
        );

    return rotate * translation; // Combined transformation
}


void PaintGLHelper::setupMatrices(GLuint shader, const glm::mat4& projMatrix, const SceneCameraData& cameraData) {
    glm::mat4 View = calculateViewMatrix(cameraData);

    glUniformMatrix4fv(glGetUniformLocation(shader, "projMatrix"), 1, GL_FALSE, &projMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "viewMatrix"), 1, GL_FALSE, &View[0][0]);

    glm::vec4 viewPosWorld = glm::inverse(View) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glUniform4fv(glGetUniformLocation(shader, "viewPos"), 1, glm::value_ptr(viewPosWorld));
}

void PaintGLHelper::setupLights(GLuint shader, const std::vector<SceneLightData>& lights) {
    int numLights = lights.size();
    glUniform1i(glGetUniformLocation(shader, "numLights"), numLights);

    for (int i = 0; i < numLights; ++i) {
        const auto& light = lights[i];
        std::string lightIndex = "lights[" + std::to_string(i) + "]";

        if (light.type == LightType::LIGHT_DIRECTIONAL) {
            glUniform4fv(glGetUniformLocation(shader, (lightIndex + ".color").c_str()), 1, glm::value_ptr(light.color));
            setupDirectionalLight(shader, lightIndex, light);
        } else {
            //for future lights
            glUniform4fv(glGetUniformLocation(shader, (lightIndex + ".color").c_str()), 1, glm::value_ptr(light.color));
            setupPointOrSpotLight(shader, lightIndex, light);
        }
    }
}

void PaintGLHelper::setupDirectionalLight(GLuint shader, const std::string& lightIndex, const SceneLightData& light) {
    glUniform3fv(glGetUniformLocation(shader, (lightIndex + ".direction").c_str()), 1, glm::value_ptr(light.dir));
    glUniform1i(glGetUniformLocation(shader, (lightIndex + ".isDirectional").c_str()), 1);

    // No attenuation for directional lights
    glUniform3f(glGetUniformLocation(shader, (lightIndex + ".attenuation").c_str()), light.function.x, light.function.y, light.function.z);
    glUniform1f(glGetUniformLocation(shader, (lightIndex + ".angle").c_str()), 0.0f);
    glUniform1f(glGetUniformLocation(shader, (lightIndex + ".penumbra").c_str()), 0.0f);
}

void PaintGLHelper::setupPointOrSpotLight(GLuint shader, const std::string& lightIndex, const SceneLightData& light) {
    glUniform4fv(glGetUniformLocation(shader, (lightIndex + ".position").c_str()), 1, glm::value_ptr(light.pos));
    glUniform1i(glGetUniformLocation(shader, (lightIndex + ".isDirectional").c_str()), 0);

    // Set attenuation factors
    glUniform3fv(glGetUniformLocation(shader, (lightIndex + ".attenuation").c_str()), 1, glm::value_ptr(light.function));

    // Set angle and penumbra for spotlights
    if (light.type == LightType::LIGHT_SPOT) {
        glUniform3fv(glGetUniformLocation(shader, (lightIndex + ".direction").c_str()), 1, glm::value_ptr(light.dir));
        glUniform1f(glGetUniformLocation(shader, (lightIndex + ".angle").c_str()), light.angle);
        glUniform1f(glGetUniformLocation(shader, (lightIndex + ".penumbra").c_str()), light.penumbra);
    } else {
        glUniform3fv(glGetUniformLocation(shader, (lightIndex + ".direction").c_str()), 1, glm::value_ptr(light.dir));
        glUniform1f(glGetUniformLocation(shader, (lightIndex + ".angle").c_str()), 0.0f);
        glUniform1f(glGetUniformLocation(shader, (lightIndex + ".penumbra").c_str()), 0.0f);
    }
}

void PaintGLHelper::renderShapes(GLuint shader, const std::vector<RenderShapeData>& shapes, const SceneGlobalData& globalData) {
    for (const auto& shape : shapes) {
        // Set per-shape uniforms
        glUniformMatrix4fv(glGetUniformLocation(shader, "modelMatrix"), 1, GL_FALSE, &shape.ctm[0][0]);
        glUniform4fv(glGetUniformLocation(shader, "shapeColor"), 1, glm::value_ptr(shape.primitive.material.cAmbient * globalData.ka));
        glUniform4fv(glGetUniformLocation(shader, "shapeDiffuse"), 1, glm::value_ptr(shape.primitive.material.cDiffuse * globalData.kd));
        glUniform4fv(glGetUniformLocation(shader, "shapeSpecular"), 1, glm::value_ptr(shape.primitive.material.cSpecular * globalData.ks));
        glUniform1f(glGetUniformLocation(shader, "shininess"), shape.primitive.material.shininess);

        // Bind and draw the shape
        glBindVertexArray(shape.vao);
        glDrawArrays(GL_TRIANGLES, 0, shape.dataSize / 6);
        glBindVertexArray(0);
    }
}

