#include <string>
#include <GL/glew.h>
#include "../../lib/tiny_gltf.h"
#include "utils/sceneparser.h"

class modelloader
{
public:
    modelloader();
    static tinygltf::Model loadModel(std::string filePath);
    static void loadArrayToVBO(GLuint& vbo, GLuint& vao, std::vector<float> data);
    static void updateVBO(GLuint vbo, const std::vector<float>& data);
    static void renderModel(GLuint &shader, GLuint &vao, RenderData renderData, std::vector<float> data);

    //no animations, initialized global transforms to identity
    static std::vector<float> LoadVerticesNormals(tinygltf::Model &model);

    static std::vector<float> LoadVerticesNormals(tinygltf::Model &model, std::vector<glm::mat4> &globalTransforms);

    /**
     * @brief LoadGLB
     * @param filename - glb filepath
     * @param model - pointer to where the model will be stored
     * @return 0 for success, -1 on failure
     */
    static int LoadGLB(const std::string &filename, tinygltf::Model &model);

    static void ApplyAnimations(const tinygltf::Model &model, float timeStep, std::vector<glm::mat4> &globalTransforms, int animationIndex);
    static void UpdateAnimation(float &currentTime, float deltaTime, const tinygltf::Model &model, const tinygltf::Animation &animation);


    static glm::vec3 InterpolateTranslation(const tinygltf::AnimationSampler &sampler, float timeStep, const tinygltf::Model &model);
    static glm::quat InterpolateRotation(const tinygltf::AnimationSampler &sampler, float timeStep, const tinygltf::Model &model);
    static glm::vec3 InterpolateScale(const tinygltf::AnimationSampler &sampler, float timeStep, const tinygltf::Model &model);

    static void InterpolateAnimation(glm::mat4 &transform, const tinygltf::AnimationChannel &channel,
                                     const tinygltf::AnimationSampler &sampler, float timeStep);
    static void ApplyAnimationToNode(const tinygltf::Model &model, int nodeIndex, const glm::mat4 &parentTransform,
                         std::vector<glm::mat4> &globalTransforms, const tinygltf::Animation &animation, float timeStep);
};
