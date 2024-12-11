#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include "glm/fwd.hpp"
#include "terrain/coral.h"
#include <vector>
class TerrainGenerator
{
public:
    TerrainGenerator();
    std::vector<coral*> generateCoralClusters();
private:
    // Pre-loaded data
    std::vector<float> tallCoralData;
    std::vector<float> brainCoralData;
    std::vector<float> genericCoralData;

};

#endif // TERRAINGENERATOR_H
