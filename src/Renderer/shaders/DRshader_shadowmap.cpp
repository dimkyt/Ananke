#include "Renderer\shaders\DRshader_shadowmap.h"
#include "Renderer\glsl.h"

// System includes
#include <stdlib.h>
#include <string>

DRshader_shadowmap::DRshader_shadowmap()
{
}

DRshader_shadowmap::~DRshader_shadowmap()
{
	glDeleteProgram(shaderID);
}

void DRshader_shadowmap::init()
{
	shaderID = LoadShaders(std::string("shadowmap.vert").c_str(), std::string("shadowmap.frag").c_str());
	uniform_MVP = glGetUniformLocation(shaderID, "MVP");
}