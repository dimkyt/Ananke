#version 330 core

in vec2 UV;

out float avgLuminance;

uniform sampler2D tex_color;

void main()
{
	vec3 color = texture(tex_color, UV).xyz;
	float luminance = dot(color, vec3(.27, .67, .06));
	avgLuminance = log(luminance + 0.0001);
}