#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>
#include "shapes/Cube.h"
#include "shapes/Cylinder.h"
#include "shapes/Sphere.h"
#include "shapes/Cone.h"
#include "settings.h"
#include <chrono>
#include <iostream>

Cone cone;
Cylinder cylinder;
Sphere sphere;
Cube cube;


glm::mat4 getTransformationMatrix(SceneTransformation& transformation) {
    glm::mat4 result = glm::mat4(1.0f); // Start with the identity matrix
    switch (transformation.type) {
    case TransformationType::TRANSFORMATION_TRANSLATE:
        result = glm::translate(result, transformation.translate); // Apply translation
        break;
    case TransformationType::TRANSFORMATION_SCALE:
        result = glm::scale(result, transformation.scale); // Apply scaling
        break;
    case TransformationType::TRANSFORMATION_ROTATE:
        result = glm::rotate(result, transformation.angle, transformation.rotate); // Apply rotation
        break;
    case TransformationType::TRANSFORMATION_MATRIX:
        result = transformation.matrix; // Use the custom matrix
        break;
    }
    return result;
}

void dfsTree(SceneNode* scene, glm::mat4 ctm, std::vector<RenderShapeData>& renderShapes, std::vector<SceneLightData>& lights) {
    if (scene == nullptr) return;

    for (auto it = scene->transformations.begin(); it != scene->transformations.end(); ++it) {
        ctm = ctm*getTransformationMatrix(*(*it));
    }

    if (scene->children.empty()) {
        for (const auto& primitive : scene->primitives) {
            RenderShapeData shapeData;
            shapeData.primitive = *primitive;
            shapeData.ctm = ctm;
            shapeData.inversectm = glm::inverse(ctm);
            renderShapes.push_back(shapeData);
        }

        for (const auto& light : scene->lights) {
            SceneLightData lightData;
            lightData.id = light->id;
            lightData.type = light->type;
            lightData.color = light->color;
            lightData.function = light->function;

            if (light->type == LightType::LIGHT_POINT || light->type == LightType::LIGHT_SPOT) {
                lightData.pos = ctm * glm::vec4(0.0f, 0.f, 0.f, 1.f);
            }
            if (light->type == LightType::LIGHT_SPOT|| light->type == LightType::LIGHT_DIRECTIONAL) {
                lightData.dir = ctm * light->dir;
                lightData.penumbra = light->penumbra;
                lightData.angle = light->angle;
            }


            lights.push_back(lightData);
        }
        return;
    }

    for (SceneNode* child : scene->children) {
        dfsTree(child, ctm, renderShapes, lights);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // TODO: Use your Lab 5 code here
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();
    SceneNode* root = fileReader.getRootNode();
    std::vector<RenderShapeData> renderShapes;
    std::vector<SceneLightData> renderLights;
    dfsTree(root, glm::mat4(1.0f), renderShapes, renderLights);
    renderData.shapes = renderShapes;
    renderData.lights = renderLights;
    return true;
}
