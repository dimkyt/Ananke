#version 330 core

in vec2 UV;

uniform float width_inv;
uniform float height_inv;
uniform mat4 P_inv;
uniform mat4 matrix_P;
uniform vec2 noise_scale;
uniform float kernel_size;
uniform vec3 kernel[100];
uniform float radius;

uniform sampler2D tex_depth;
uniform sampler2D tex_normal;
uniform sampler2D tex_noise;

// Ouput data
out vec3 result;

vec3 depthToPosition(float depth_clip)
{
	vec4 eye;             //EYE-space position
	vec4 clip;			  //CLIP-space position
 
	clip = vec4(
    2.0 * gl_FragCoord.x * width_inv - 1.0,
    2.0 * gl_FragCoord.y * height_inv - 1.0,
    depth_clip,
    1.0);

	eye = P_inv * clip;
	eye /= eye.w;
 
	return eye.xyz;
}


void main()
{
	float depth = texture(tex_depth, UV).x;
	vec3 pos = depthToPosition(2.0*depth - 1.0);
	vec3 normal = texture(tex_normal, UV).xyz;

	vec3 rvec = texture(tex_noise, UV * noise_scale).xyz;

	vec3 tangent = normalize(rvec - normal * dot(normal, rvec));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for (int i = 0; i < kernel_size; ++i)
	{
		//get sample position:
		vec3 sample = tbn * kernel[i];
		sample = sample * radius + pos;
  
		//project sample position:
	    vec4 offset = vec4(sample, 1.0);
	    offset = matrix_P * offset;
	    offset.xyz /= offset.w;
	    offset.xy = offset.xy * 0.5 + 0.5;
  
		//get sample depth:
	    float sampleDepth = texture(tex_depth, offset.xy).r;
		float sampleDepth_clip = sampleDepth*2.0 - 1.0;
		vec3 sample_pos = depthToPosition(sampleDepth_clip);
  
		//range check & accumulate:
	    float rangeCheck= abs(pos.z - sample_pos.z) < radius ? 1.0 : 0.0;
	    occlusion += (sampleDepth_clip <= offset.z ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion/kernel_size);

	result = vec3(occlusion, occlusion, occlusion);
}