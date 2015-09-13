#ifndef _DR_SHADER_AA_
#define _DR_SHADER_AA_

#include <GL/glew.h>

class DRshader_AA
{
public:
	GLuint shaderID,
		   uniform_pixel_size,
		   uniform_weight,
		   uniform_sampler_normal,
		   uniform_sampler_render;

	DRshader_AA();
	~DRshader_AA();

	void init();
	void AApass(unsigned win_width, unsigned win_height, GLuint normal_map,
				GLuint render_FBO, GLuint render_map, GLuint screen_quad_vaoId);
};

#endif