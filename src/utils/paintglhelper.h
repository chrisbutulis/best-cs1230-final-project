#ifndef PAINTGLHELPER_H
#define PAINTGLHELPER_H

#include "realtime.h"
class PaintGLHelper {
public:
    static glm::mat4 setupMatrices(GLuint shader, const glm::mat4& projMatrix, const SceneCameraData& cameraData);
    static void setupLights(GLuint shader, const std::vector<SceneLightData>& lights, player::PlayerType playerType);
    static void renderShapes(GLuint shader, const std::vector<RenderShapeData>& shapes, const SceneGlobalData& globalData);
    static void renderCoral(GLuint shader, const std::vector<coral*> coralData, const RenderData& renderData);
    static void renderFish(GLuint shader, fish fish, const RenderData& renderData);
private:
    static void setupDirectionalLight(GLuint shader, const std::string& lightIndex, const SceneLightData& light);
    static void setupPointOrSpotLight(GLuint shader, const std::string& lightIndex, const SceneLightData& light);
};


#endif // PAINTGLHELPER_H
