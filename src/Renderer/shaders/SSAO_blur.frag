#version 330 core

in vec2 UV;

uniform float width_inv;
uniform float height_inv;

uniform sampler2D tex_SSAO;

out vec3 oResult;

void main()
{
	vec2 off_coords = vec2(width_inv, height_inv);
	float result = 0.0;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			vec2 offset = vec2(off_coords.x * float(j), off_coords.y * float(i));
			result += texture(tex_SSAO, UV + offset).r;
		}
	}
 
	result /= 16.0;
	oResult = vec3(result, result, result);
}