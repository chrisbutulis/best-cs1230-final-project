#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class objloader
{
public:
    bool loadOBJ(
        const char * path,
        std::vector < glm::vec3 > & out_vertices,
        std::vector < glm::vec2 > & out_uvs,
        std::vector < glm::vec3 > & out_normals
        );
};

