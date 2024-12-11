#include "genericcoral.h"

GenericCoral::GenericCoral() {
    // Ambient reflection coefficient (low intensity blue)
    cAmbient = glm::vec4(0.0f, 0.1f, 0.2f, 1.0f);

    // Diffuse reflection coefficient (bright blue)
    cDiffuse = glm::vec4(0.0f, 0.5f, 1.0f, 1.0f);

    // Specular reflection coefficient (light blue-white for subtle highlights)
    cSpecular = glm::vec4(0.3f, 0.3f, 0.5f, 1.0f);

    // Shininess exponent (moderate shininess)
    shininess = 32.0f;
}
