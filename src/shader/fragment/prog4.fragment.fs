#version 330 core

in vec3 fNormal;
in vec3 fragPos;

out vec4 color;

uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    float ambientStrength = 0.15;
    vec3 ambient = lightColor * ambientStrength;
    vec3 norm = normalize(fNormal);
    vec3 lightDirection = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;
    color = vec4((diffuse + ambient) * objColor, 1.0);
}