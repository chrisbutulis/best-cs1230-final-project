#include "genericcoral.h"

GenericCoral::GenericCoral() {
    // Ambient reflection coefficient (low intensity purple)
    cAmbient = glm::vec4(0.1f, 0.0f, 0.1f, 1.0f);

    // Diffuse reflection coefficient (bright purple)
    cDiffuse = glm::vec4(0.5f, 0.0f, 0.5f, 1.0f);

    // Specular reflection coefficient (light purple-white for subtle highlights)
    cSpecular = glm::vec4(0.7f, 0.5f, 0.7f, 1.0f);

    // Shininess exponent (moderate shininess)
    shininess = 32.0f;
}
