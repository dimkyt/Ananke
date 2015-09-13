#ifndef _DR_SHADER_FXAA_
#define _DR_SHADER_FXAA_

#include <GL/glew.h>

class DRshader_FXAA
{
public:
	GLuint shaderID,
		   uniform_pixel_size,
		   uniform_sampler_color;

	DRshader_FXAA();
	~DRshader_FXAA();

	void init();
	void FXAApass(unsigned win_width, unsigned win_height,
				   GLuint render_FBO, GLuint render_map, GLuint screen_quad_vaoId);
};

#endif