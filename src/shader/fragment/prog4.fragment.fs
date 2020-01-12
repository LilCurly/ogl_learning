#version 330 core

in vec3 fNormal;
in vec3 fragPos;

out vec4 color;

uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    float ambientStrength = 0.15;
    float specularStrength = 0.5;
    vec3 ambient = lightColor * ambientStrength;
    vec3 norm = normalize(fNormal);
    vec3 lightDirection = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    color = vec4((ambient + diffuse + specular) * objColor, 1.0);
}