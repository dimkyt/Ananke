#ifndef _DR_SHADER_ACC_PASS_
#define _DR_SHADER_ACC_PASS_

#include <GL/glew.h>

class DRshader_accPass
{
public:
	GLuint shaderID,
		     uniform_use_SSAO,
		     uniform_use_bloom,
		     uniform_sampler_SSAO,
		     uniform_sampler_bloom,
		     uniform_sampler_color;

	DRshader_accPass();
	~DRshader_accPass();
	void init();
};

#endif