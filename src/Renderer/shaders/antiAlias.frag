#version 330 core

in vec2 UV;

out vec4 color;

uniform vec2 pixel_size;
uniform float weight;

uniform sampler2D tex_normal;
uniform sampler2D tex_render;

void main()
{
	const vec2 offset[8] = vec2[8]
	(
		vec2(-1.,1.), vec2(1.,-1.), vec2(-1.,-1.), vec2(1.,1.),
		vec2(-1.,.0), vec2(1.,.0), vec2(.0,-1.), vec2(.0,1.)
	);

	float factor = .0;
	vec4 normal = texture(tex_normal, UV);

	for(int i=0; i<4; i++)
	{
		vec4 t = texture(tex_normal, UV+offset[i]*pixel_size);
		t -= normal;
		factor += dot(t,t);
	}
	factor = min(1.0,factor)*weight;

	color = vec4(.0, .0, .0, .0);
	for(int i=0; i<8; i++)
	{
		color += texture(tex_render, UV + offset[i]*pixel_size*factor);
	}

	color += 2.0*texture(tex_render, UV);
	color = color/10.;
}