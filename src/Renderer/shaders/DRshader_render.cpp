#include "Renderer\shaders\DRshader_render.h"
#include "Renderer\glsl.h"

// System includes
#include <stdlib.h>
#include <string>

DRshader_render::DRshader_render()
{
}

DRshader_render::~DRshader_render()
{
	glDeleteProgram(shaderID);
}

void DRshader_render::init()
{
	shaderID = LoadShaders(std::string("Render.vert").c_str(), std::string("Render.frag").c_str());
	uniform_sampler_color = glGetUniformLocation(shaderID, "tex_color");
}