#version 450 core

in vec2 UV;

out vec4 color;

uniform float use_SSAO;

uniform sampler2D tex_SSAO;
uniform sampler2D tex_color;

void main()
{
	color = texture(tex_color, UV);
}