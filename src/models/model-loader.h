#include <string>
#include <GL/glew.h>
#include "../../lib/tiny_gltf.h"
#include "utils/sceneparser.h"

class modelloader
{
public:
    modelloader();
    static tinygltf::Model loadModel(std::string filePath);
    static std::vector<float> loadGLB(GLuint& terrain_vbo, GLuint& terrain_vao, std::string filePath);
    static void renderModel(GLuint &shader, GLuint &vao, RenderData renderData, std::vector<float> data);
};
