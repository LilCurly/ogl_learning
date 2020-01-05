#version 330 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 color_s;

out vec3 color_f;

void main()
{
    gl_Position = vec4(position, 1.0);
    color_f = color_s;
}