#version 330 core

in vec3 fNormal;
in vec3 fragPos;
in vec2 texCoord;

out vec4 color;

struct Material {
    sampler2D specular;
    sampler2D diffusal;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    float innerAngle;
    float outerAngle;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material mat;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient * vec3(texture(mat.diffusal, texCoord));

    vec3 lightDirection = normalize(light.position - fragPos);
    float theta = dot(lightDirection, normalize(-light.direction));
    float epsilon = light.innerAngle - light.outerAngle;
    float itensity = clamp((theta - light.outerAngle) / epsilon, 0.0, 1.0);
    vec3 norm = normalize(fNormal);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(mat.diffusal, texCoord)));

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(mat.specular, texCoord)));

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    diffuse *= itensity;
    specular *= itensity;

    color = vec4(ambient + diffuse + specular, 1.0);
}