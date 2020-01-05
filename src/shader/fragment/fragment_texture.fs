#version 330 core

in vec3 color_f;
in vec2 tex_frag_coord;

uniform sampler2D tex;

out vec4 color;

void main()
{
    color = texture(tex, tex_frag_coord);
}