#version 330 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

out vec3 fNormal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelNormal;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    fragPos = vec3(model * vec4(position, 1.0));
    fNormal = mat3(modelNormal) * normal;
}