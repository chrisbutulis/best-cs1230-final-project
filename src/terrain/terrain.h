#include <string>
#include "../../lib/tiny_gltf.h"

class terrain
{
public:
    terrain();
    static tinygltf::Model loadModel(std::string filePath);
};
