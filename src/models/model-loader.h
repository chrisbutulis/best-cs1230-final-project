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
    static void renderModel(GLuint &shader, GLuint &vao, RenderData renderData, std::vector<float> data);
    // static std::vector<float> LoadGLBVerticesNormals(const std::string &filename);

    static std::vector<float> LoadVerticesNormals(tinygltf::Model &model);
    /**
     * @brief LoadGLB
     * @param filename - glb filepath
     * @param model - pointer to where the model will be stored
     * @return 0 for success, -1 on failure
     */
    static int LoadGLB(const std::string &filename, tinygltf::Model &model);
};
