#version 450 core

in vec3 normal;
in vec3 diff_mat;
in vec3 spec_mat;
in float Ns;
in vec3 tex_coords;
in float has_diffuse_tex;
in float has_specular_tex;

uniform mat4 MV_inv_trans;

uniform sampler2D diffuse_tex_sampler;
uniform sampler2D specular_tex_sampler;

// Ouput data
layout(location = 0) out vec4 norm;
layout(location = 1) out vec4 diffuse;
layout(location = 2) out vec4 specular;


void main()
{	
	norm = MV_inv_trans*vec4(normal, 1.0);
	diffuse = has_diffuse_tex==0.0 ? vec4(diff_mat, 1.0) : vec4(diff_mat,1.0)*texture(diffuse_tex_sampler, tex_coords.st);
	specular = has_specular_tex==0.0 ? vec4(spec_mat, Ns) : vec4(spec_mat,Ns)*vec4(texture(specular_tex_sampler, tex_coords.st).xyz, 1.);
}