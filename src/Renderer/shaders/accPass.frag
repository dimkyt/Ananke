#version 450 core

in vec2 UV;

out vec4 color;

uniform float use_SSAO;
uniform float use_bloom;

uniform sampler2D tex_SSAO;
uniform sampler2D tex_bloom;
uniform sampler2D tex_color;

void main()
{
	vec4 acc_color = texture(tex_color, UV);

	acc_color = use_bloom==1.0 ? mix(acc_color, texture(tex_bloom, UV), 0.01) : acc_color;
	acc_color = use_SSAO==1.0 ? acc_color*texture(tex_SSAO, UV).r : acc_color;

	color = acc_color;
}