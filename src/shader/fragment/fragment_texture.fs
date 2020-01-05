#version 330 core

in vec3 color_f;
in vec2 tex_frag_coord;

uniform sampler2D firstTex;
uniform sampler2D secondTex;
uniform float mixParameter;

out vec4 color;

void main()
{
    color = mix(texture(firstTex, tex_frag_coord), texture(secondTex, tex_frag_coord), mixParameter);
}