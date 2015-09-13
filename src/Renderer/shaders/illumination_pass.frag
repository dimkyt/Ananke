#version 330 core

in vec2 UV;

out vec4 color;

uniform float width_inv;
uniform float height_inv;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform mat4 P_inv;
uniform mat4 MV_inv;
uniform mat4 MVP_shadow;

uniform sampler2D tex_normal;
uniform sampler2D tex_diffuse;
uniform sampler2D tex_specular;
uniform sampler2D tex_depth;
uniform sampler2D tex_shadow;

float ShadowSampler(sampler2D sm, vec2 sample, float ref)
{
	ivec2 is = textureSize(sm,0);
	float shadeFactor = 0; 
	vec2 offset = vec2(0,0);
	for (int i=-1;i<2;i++)
	{
		for (int j=-1;j<2;j++)
		{
		   vec2 nsp = sample+vec2(float(i)/is.x,float(j)/is.y);
		   vec2 sp = clamp (nsp, vec2(0,0), vec2(1,1));
		   float depth = 2*texture(sm,sp).x-1;
		   int inside = (nsp-sp)==vec2(0.0,0.0)?1:0;
		   inside*=ref>0?1:0;
		   shadeFactor += (ref<(depth+0.002)?1.0:0.0)*inside;
		}
	}

	return shadeFactor/9.0;
}

vec3 depthToPosition(float depth)
{
	vec4 eye;             //EYE-space position
	vec4 clip;			  //CLIP-space position
	
	clip = vec4(
    2.0 * gl_FragCoord.x * width_inv - 1.0,
    2.0 * gl_FragCoord.y * height_inv - 1.0,
    2.0 * depth - 1.0,
    1.0);
	
	eye = P_inv * clip;
	eye /= eye.w;
 
	return eye.xyz;
}

void main()
{
	float depth = texture(tex_depth, UV).x;
	vec3 pos = depthToPosition(depth);
	vec3 normal = texture(tex_normal, UV).xyz;
	vec4 Kd = texture(tex_diffuse, UV);
	vec4 Ks = texture(tex_specular, UV);

	vec3 light_dir = normalize(lightPos - pos);
	float costh = max(dot(light_dir, normal), .0);
	vec4 Id = vec4(lightColor, 1.) * Kd * costh;

	vec3 v = normalize(pos);
	vec3 h = normalize(v + light_dir);
	costh = max(dot(h, normal), .0);
	vec4 Is = vec4(lightColor, 1.) * vec4(Ks.xyz, 1.) * min(pow(costh, Ks.w), .0);

	vec4 shadow_coords = MVP_shadow * MV_inv * vec4(pos, 1.0);
	vec2 UV_shadow = (shadow_coords.xy/shadow_coords.w + 1.0)/2.0;
	float shade_factor = ShadowSampler(tex_shadow, UV_shadow, shadow_coords.z/shadow_coords.w);
	//shade_factor = 1.0;

	color = shade_factor*(Id + Is*0.25);
}