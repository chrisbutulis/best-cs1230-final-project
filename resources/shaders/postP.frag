#version 330 core

in vec2 uv; // Task 16: Create a UV coordinate in variable
uniform sampler2D s2D; // Task 8: Add a sampler2D uniform
uniform bool postP;
uniform bool postP2;
out vec4 fragColor;

const float kernel[25] = float[](
    0.04, 0.04, 0.04, 0.04, 0.04,
    0.04, 0.04, 0.04, 0.04, 0.04,
    0.04, 0.04, 0.04, 0.04, 0.04,
    0.04, 0.04, 0.04, 0.04, 0.04,
    0.04, 0.04, 0.04, 0.04, 0.04
);

void main()
{
    vec2 texelSize = 1.0 / textureSize(s2D, 0);

    vec4 baseColor = texture(s2D, uv);
    fragColor = baseColor;

    // Task 33: Invert color if postP is true

    if (postP2) {
        vec4 blurredColor = vec4(0.0);
        int index = 0;
        for (int i = -2; i <= 2; ++i) {
            for (int j = -2; j <= 2; ++j) {
                vec2 offset = vec2(float(i), float(j)) * texelSize;
                blurredColor += texture(s2D, uv + offset) * kernel[index];
                index++;
            }
        }
        fragColor = blurredColor;
    }

    if (postP) {
        fragColor.r = 1.0 - fragColor.r;
        fragColor.g = 1.0 - fragColor.g;
        fragColor.b = 1.0 - fragColor.b;
    }
}
