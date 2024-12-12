#ifndef CORAL_H
#define CORAL_H

#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

class coral
{
public:
    coral();
    virtual ~coral();

    std::vector<float> coralData;
    glm::mat4 ctm;
    GLuint vbo;
    GLuint vao;

    glm::vec4 cAmbient;
    glm::vec4 cDiffuse;
    glm::vec4 cSpecular;
    float shininess;
};

#endif // CORAL_H
