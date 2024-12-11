#version 330 core
in vec2 uv;

uniform sampler2D fontTexture; // Bitmap font texture
uniform vec3 textColor;        // Text color

out vec4 fragColor;

void main() {
    float alpha = texture(fontTexture, uv).r; // Assume red channel stores the glyph's alpha
    fragColor = vec4(textColor, alpha);       // Use the alpha channel for text visibility
    if (alpha < 0.1) discard;                // Discard nearly transparent pixels
}
