#include "Renderer\shaders\DRshader_Gpass.h"
#include "Renderer\glsl.h"

// System includes
#include <stdlib.h>
#include <string>

DRshader_Gpass::DRshader_Gpass()
{
}

DRshader_Gpass::~DRshader_Gpass()
{
	glDeleteProgram(shaderID);
}

void DRshader_Gpass::init()
{
	shaderID = LoadShaders(std::string("G_pass.vert").c_str(), std::string("G_pass.frag").c_str());
	uniform_MVP = glGetUniformLocation(shaderID, "MVP");
	uniform_MV_inv_trans = glGetUniformLocation(shaderID, "MV_inv_trans");
	uniform_diffuseMat = glGetUniformLocation(shaderID, "diffuse_mat");
	uniform_specularMat = glGetUniformLocation(shaderID, "specular_mat");
	uniform_Ns = glGetUniformLocation(shaderID, "shininess");
	uniform_has_diffuse_tex = glGetUniformLocation(shaderID, "in_has_diffuse_tex");
	uniform_has_specular_tex = glGetUniformLocation(shaderID, "in_has_specular_tex");
	uniform_sampler_diffuse = glGetUniformLocation(shaderID, "diffuse_tex_sampler");
	uniform_sampler_specular = glGetUniformLocation(shaderID, "specular_tex_sampler");
}