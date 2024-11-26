#version 330 core

in vec3 worldPos;
in vec3 worldNormal;

out vec4 fragColor;

uniform vec4 shapeColor;        // Ambient color
uniform vec4 shapeDiffuse;      // Diffuse color
uniform vec4 shapeSpecular;     // Specular color
uniform float shininess;        // Shininess factor for specular
uniform vec4 viewPos;           // Camera position in world coordinates

// Light structure
struct Light {
    vec4 color;                 // Light color
    vec4 position;              // Position for point lights
    vec3 direction;             // Direction for directional lights
    bool isDirectional;         // Light type: directional or point
    float angle;
    float penumbra;
    vec3 attenuation;
};

uniform int numLights;          // Number of active lights
uniform Light lights[10];       // Array of lights, assuming a maximum of 10 lights

void main() {
    vec3 norm = normalize(worldNormal);
    vec3 viewDir = normalize(vec3(viewPos) - worldPos);

    vec4 accumulatedColor = vec4(0.0);
    // Ambient component
    vec4 ambient = shapeColor;
    accumulatedColor += ambient;
    for (int i = 0; i < numLights; i++) {
        Light light = lights[i];

        float atten = 1.0;
        float distance = 1.0;
        vec3 effectiveLightDir;

        float fallOff = 1.f;

        // For when I need to handle other lights
        if (light.penumbra != 0.0f) {
            vec3 lightDir = normalize(vec3(light.position) - worldPos);
            float x = acos(dot(lightDir, normalize(-vec3(light.direction))));
            float innerAngle = light.angle - light.penumbra;

            if (abs(x) >= light.angle) {
                continue;
            }

            if (x > innerAngle) {
                float t = (x - innerAngle) / light.penumbra;
                fallOff = 1.0f + (2.0f * t * t * t - 3.0f * t * t);
            } else {
                fallOff = 1.0f;
            }
        }


        if (light.isDirectional) {
            effectiveLightDir = normalize(-light.direction);
            atten = 1.0f;
        }
        else {
        //     // For when I need to handle other lights
            distance = length(vec3(light.position) - worldPos);
            effectiveLightDir = normalize(vec3(light.position) - worldPos);
            atten = 1.0f / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * distance * distance);
            atten = min(1.0f, atten);

        }


        // Diffuse component
        float diffIntensity = max(dot(norm,effectiveLightDir), 0.0);
        vec4 diffuse = shapeDiffuse * light.color * diffIntensity*fallOff*atten;

        // Specular component
        vec3 reflectDir = reflect(-effectiveLightDir, norm);

        float specIntensity = 1;
        if (shininess != 0.f){
            specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        }

        vec4 specular = shapeSpecular * light.color * specIntensity*atten*fallOff;

        // Accumulate this light's contribution
        accumulatedColor +=  diffuse + specular;
    }

    // Final fragment color
    fragColor = accumulatedColor;

}
