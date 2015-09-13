#ifndef _DR_SHADER_RENDER_
#define _DR_SHADER_RENDER_

#include <GL/glew.h>

class DRshader_render
{
public:
	GLuint shaderID,
		   uniform_sampler_color;

	DRshader_render();
	~DRshader_render();
	void init();
};

#endif