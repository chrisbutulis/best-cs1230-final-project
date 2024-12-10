#include "terrain/coral.h"

class TallCoral : public coral {
public:
    TallCoral() {
        // Ambient reflection coefficient (muted orange)
        cAmbient = glm::vec4(0.1f, 0.05f, 0.0f, 1.0f);

        // Diffuse reflection coefficient (bright orange)
        cDiffuse = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);

        // Specular reflection coefficient (warm white for subtle highlights)
        cSpecular = glm::vec4(0.6f, 0.4f, 0.2f, 1.0f);

        // Shininess exponent (moderate to high shininess)
        shininess = 64.0f;
    }
    virtual ~TallCoral() {}
};
