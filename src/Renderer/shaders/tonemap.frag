#version 330 core

in vec2 UV;

out vec4 color;

uniform float avgLogLuminance;

uniform sampler2D tex_color;

void main()
{
	float a = 0.8;
	vec3 color_init = texture(tex_color, UV).xyz;
    vec3 lumVector = vec3(0.27, 0.67, 0.06);
	float grayLum = (1./avgLogLuminance) * a * dot(color_init, lumVector);
	float Ld = grayLum/(1+grayLum);
	color = vec4(color_init * Ld, 1);
}