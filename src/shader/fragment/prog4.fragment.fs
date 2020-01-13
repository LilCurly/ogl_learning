#version 330 core

in vec3 fNormal;
in vec3 fragPos;

out vec4 color;

struct Material {
    vec3 ambient;
    vec3 specular;
    vec3 diffusal;
    float shininess;
};

uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material mat;

void main()
{

    vec3 ambient = lightColor * mat.ambient;

    vec3 norm = normalize(fNormal);
    vec3 lightDirection = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = lightColor * (diff * mat.diffusal);

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    vec3 specular = lightColor * (spec * mat.specular);
    color = vec4((ambient + diffuse + specular), 1.0);
}