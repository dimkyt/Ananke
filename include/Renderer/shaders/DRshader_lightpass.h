#ifndef _DR_SHADER_LIGHTPASS_
#define _DR_SHADER_LIGHTPASS_

#include <GL/glew.h>

class DRshader_lightpass
{
public:
	GLuint shaderID,
		   uniform_width_inv,
		   uniform_height_inv,
		   uniform_lightPos,
		   uniform_lightColor,
		   uniform_MVP_shadow,
		   uniform_P_inv,
		   uniform_MV_inv,
		   uniform_sampler_normal,
		   uniform_sampler_diffuse,
		   uniform_sampler_specular,
		   uniform_sampler_depth,
		   uniform_sampler_shadow;

	DRshader_lightpass();
	~DRshader_lightpass();
	void init();
};

#endif