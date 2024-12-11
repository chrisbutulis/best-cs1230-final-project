#version 330 core

in vec2 uv;
uniform sampler2D s2D;
uniform vec3 textColor;
out vec4 fragColor;

void main() {
    float alpha = texture(s2D, uv).r;

    fragColor = vec4(textColor, alpha);

    if (alpha == 0.0) {
        discard;
    }
}
