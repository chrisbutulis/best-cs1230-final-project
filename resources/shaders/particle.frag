#version 330 core
out vec4 fragColor;
uniform vec4 particleColor;
uniform float life;
void main() {
    // Blend color to white (1.0f) based on particle life
        vec3 color = mix(particleColor.rgb, vec3(1.0f, 1.0f, 1.0f), 1.0f - life);  // Blend from original color to white

        // Reduce transparency (alpha) as life decreases
//        float alpha = 1.0f - life;  // Make particle more transparent as life decreases

        // Output the final color with alpha for transparency
        fragColor = vec4(color, life);
}
