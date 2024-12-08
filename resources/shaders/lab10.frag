#version 330 core

// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader

in vec3 worldPos;
in vec3 worldNormal;

// Task 10: declare an out vec4 for your output color

out vec3 fragColor;

// Task 12: declare relevant uniform(s) here, for ambient lighting
uniform vec4 shapeDiffuse;      // Diffuse color
uniform vec4 shapeSpecular;     // Specular color
uniform float shininess;        // Shininess factor for specular
uniform vec4 viewPos;           // Camera position in world coordinates


// Task 13: declare relevant uniform(s) here, for diffuse lighting

// Task 14: declare relevant uniform(s) here, for specular lighting

void main() {
    // Remember that you need to renomalize vectors here if you want them to be normalized

    vec3 L = viewPos-worldPos;
    vec3 E = worldPos - viewPos;
    fragColor = vec3(ka) +
            vec3(shapeDiffuse) * clamp(dot(normalize(worldNormal), normalize(L)), 0.0, 1.0) +
            vec3(shapeSpecular) * pow(clamp(dot(normalize(E), normalize(reflect(L, normalize(worldNormal)))), 0.0, 1.0), shininess);

    // Task 13: add diffuse component to output color

    // Task 14: add specular component to output color
}
