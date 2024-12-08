#include <string>
#include <GL/glew.h>
#include "../../lib/tiny_gltf.h"

class terrain
{
public:
    terrain();
    static tinygltf::Model loadModel(std::string filePath);
    static std::vector<float> getTerrainData(GLuint& terrain_vbo, GLuint& terrain_vao);
};
