#include "Renderer\shaders\DRshader_lightpass.h"
#include "Renderer\glsl.h"

// System includes
#include <stdlib.h>
#include <string>

DRshader_lightpass::DRshader_lightpass()
{
}

DRshader_lightpass::~DRshader_lightpass()
{
	glDeleteProgram(shaderID);
}

void DRshader_lightpass::init()
{
	shaderID = LoadShaders(std::string("illumination_pass.vert").c_str(), std::string("illumination_pass.frag").c_str());
	uniform_width_inv = glGetUniformLocation(shaderID, "width_inv");
	uniform_height_inv = glGetUniformLocation(shaderID, "height_inv");
	uniform_lightPos = glGetUniformLocation(shaderID, "lightPos");
	uniform_lightColor = glGetUniformLocation(shaderID, "lightColor");
	uniform_MVP_shadow = glGetUniformLocation(shaderID, "MVP_shadow");
	uniform_P_inv = glGetUniformLocation(shaderID, "P_inv");
	uniform_MV_inv = glGetUniformLocation(shaderID, "MV_inv");
	uniform_sampler_normal = glGetUniformLocation(shaderID, "tex_normal");
	uniform_sampler_diffuse = glGetUniformLocation(shaderID, "tex_diffuse");
	uniform_sampler_specular = glGetUniformLocation(shaderID, "tex_specular");
	uniform_sampler_depth = glGetUniformLocation(shaderID, "tex_depth");
	uniform_sampler_shadow = glGetUniformLocation(shaderID, "tex_shadow");
}