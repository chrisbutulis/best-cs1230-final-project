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
};

uniform int numLights;          // Number of active lights
uniform Light lights[10];       // Array of lights, assuming a maximum of 10 lights
uniform vec3 attenuation;     // Attenuation factors for point lights (constant, linear, quadratic)


void main() {
    vec3 norm = normalize(worldNormal);
    vec3 viewDir = normalize(vec3(viewPos) - worldPos);

    vec4 accumulatedColor = vec4(0.0);

    for (int i = 0; i < numLights; i++) {
        Light light = lights[i];

        // Ambient component
        vec4 ambient = shapeColor * light.color;

        float atten = 1.0;
        float distance = 1.0;
        vec3 effectiveLightDir;

        // Check if the light is directional or point

        float fallOff = 1.f;

        // For when I need to handle other lights
        // if (light.penumbra != 0.0f){
        //     vec3 normalizedDir = -normalize(vec3(light.direction));
        //     vec3 lightDir = normalize(vec3(light.position) - worldPos);
        //     float x = degrees(acos(dot(lightDir, normalize(normalizedDir))));
        //     float innerAngle = light.angle - light.penumbra;
        //     if (abs(x)>innerAngle){
        //         fallOff = 1.f - (-2.f * pow((x-innerAngle)/light.penumbra,3.f)+3.f*pow((x-innerAngle)/light.penumbra,2.f));
        //     }
        //     if (abs(x)>=light.angle){
        //         continue;
        //     }
        // }

        if (light.isDirectional) {
            effectiveLightDir = normalize(-light.direction);
            atten = 1.0f;
        }
        else {
        //     // For when I need to handle other lights
        //     distance = length(vec3(light.position) - worldPos);
        //     effectiveLightDir = normalize(vec3(light.position) - worldPos);
        //     atten = 1.0f / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance);
        //     atten = min(1.0f, atten);
            continue;
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
        accumulatedColor += ambient + diffuse + specular;
    }

    // Final fragment color
    fragColor = accumulatedColor;
}
