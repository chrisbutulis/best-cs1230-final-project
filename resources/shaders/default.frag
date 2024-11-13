#version 330 core

in vec3 worldPos;
in vec3 worldNormal;

out vec4 fragColor;

uniform vec4 shapeColor;
uniform vec4 shapeDiffuse;
uniform vec4 shapeSpecular;
uniform vec4 lightPos;
uniform float shininess;
uniform vec4 view;

void main() {
    vec3 norm = normalize(worldNormal);

    // Ambient component
    vec4 ambient = shapeColor;

    // Diffuse component
    vec3 lightDir = normalize(vec3(lightPos)-worldPos);
    float diffIntensity = max(dot(lightDir, norm), 0.0);
    vec4 diffuse = shapeDiffuse * diffIntensity;

    // Specular component
    // vec3 viewDir = normalize(vec3(view) - worldPos);
    // vec3 reflectDir = reflect(lightDir, norm);
    // float specIntensity = pow(clamp(dot(viewDir, reflectDir), 0.0, 1.0), shininess);
    // vec4 specular = vec4(shapeSpecular * specIntensity);

    fragColor = clamp(ambient + diffuse, 0.0, 1.0);
}
