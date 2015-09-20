#version 330 core

#define SAMPLES 12
#define BLUR_FACTOR 0.008

in vec2 UV;

out vec4 color_out;

uniform vec2 axis;

uniform sampler2D tex_in;
uniform sampler2D tex_color;

void main()
{
	vec4 color;
	const vec2 offset[12] = vec2[12]
	(
	vec2(-0.326212, -0.405805),
	vec2(-0.840144, -0.073580),
	vec2(-0.695914,  0.457137),
	vec2(-0.203345,  0.620716),
	vec2( 0.962340, -0.194983),
	vec2( 0.473434, -0.480026),
	vec2( 0.519456,  0.767022),
	vec2( 0.185461, -0.893124), 
	vec2( 0.507431,  0.064425), 
	vec2( 0.896420,  0.412458), 
	vec2(-0.321940, -0.932615), 
	vec2(-0.791559, -0.597705)
	);

	color = vec4(.0, .0, .0, 1.);
	for(int i=0; i<SAMPLES; i++)
	{
		vec4 t = texture(tex_in, UV+offset[i]*axis*BLUR_FACTOR);
		color += dot(t,t);
	}

	color *= 1.0/SAMPLES;
	color = axis.x==.0 ? (color + texture(tex_in, UV)) : color;

	color_out = color;
}