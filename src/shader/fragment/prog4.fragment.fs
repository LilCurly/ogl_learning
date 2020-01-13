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

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material mat;
uniform Light light;

void main()
{

    vec3 ambient = light.ambient * mat.ambient;

    vec3 norm = normalize(fNormal);
    vec3 lightDirection = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * (diff * mat.diffusal);

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    vec3 specular = light.specular * (spec * mat.specular);
    color = vec4(ambient + diffuse + specular, 1.0);
}