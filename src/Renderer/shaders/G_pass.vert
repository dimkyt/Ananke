#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 in_pos;
layout(location = 1) in vec3 in_norm;
layout(location = 2) in vec3 in_tex_coords;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform vec3 diffuse_mat;
uniform vec3 specular_mat;
uniform float shininess;
uniform float in_has_diffuse_tex;
uniform float in_has_specular_tex;

// Output data ; will be interpolated for each fragment.
out vec3 normal;
out vec3 diff_mat;
out vec3 spec_mat;
out float Ns;
out vec3 tex_coords;
out float has_diffuse_tex;
out float has_specular_tex;

void main()
{
	gl_Position =  MVP * in_pos;
	diff_mat = diffuse_mat;
	spec_mat = specular_mat;
	Ns = shininess;
	normal = in_norm;
	tex_coords = in_tex_coords;
	has_diffuse_tex = in_has_diffuse_tex;
	has_specular_tex = in_has_specular_tex;
}