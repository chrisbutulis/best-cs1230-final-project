#pragma once
#include "fish.h"
#include "lib/freetype/include/freetype2/ft2build.h"
#include FT_FREETYPE_H
#include <map>

class timer
{
public:

    struct Character {
        GLuint texture;
        glm::ivec2 size;
        glm::ivec2 verticalOffset;
        GLuint advance;
    };

    std::map<char, Character> Characters;

    GLuint vao;
    GLuint vbo;

    timer();
    void initFreeType(FT_Library& ft, FT_Face& face, const char* fontPath);
    void renderText(GLuint shaderProgram, const std::string& text, float x, float y, float scale, glm::vec3 color);

};
