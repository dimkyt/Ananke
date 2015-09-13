#include "Renderer\shaders\DRshader_accPass.h"
#include "Renderer\glsl.h"

// System includes
#include <stdlib.h>
#include <string>

DRshader_accPass::DRshader_accPass()
{
}

DRshader_accPass::~DRshader_accPass()
{
	glDeleteProgram(shaderID);
}

void DRshader_accPass::init()
{
	shaderID = LoadShaders(std::string("accPass.vert").c_str(), std::string("accPass.frag").c_str());
	uniform_use_SSAO = glGetUniformLocation(shaderID, "use_SSAO");
	uniform_use_bloom = glGetUniformLocation(shaderID, "use_bloom");
	uniform_sampler_SSAO = glGetUniformLocation(shaderID, "tex_SSAO");
	uniform_sampler_bloom = glGetUniformLocation(shaderID, "tex_bloom");
	uniform_sampler_color = glGetUniformLocation(shaderID, "tex_color");
}