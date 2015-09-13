#ifndef _DR_SHADER_TONEMAP_
#define _DR_SHADER_TONEMAP_

#include <GL/glew.h>

class DRshader_tonemap
{
private:
	GLuint logLuminance_FBO,
		     logLuminance_map;

	float avgLogLuminance,
	      maxMipmapLvl;

	unsigned win_width,
			 win_height;

public:
	GLuint shaderID,
		     shader_logLumID,
		     uniform_sampler_color,
		     uniform_sampler_color_logLum,
		     uniform_avgLogLuminance;

	DRshader_tonemap();
	~DRshader_tonemap();

	void init(unsigned width, unsigned height);
	void calcAvgLogLuminance(GLuint render_map, GLuint screen_quad_vaoId);
	void tonemap(unsigned win_width, unsigned win_height, GLuint render_FBO, GLuint render_map, GLuint screen_quad_vaoId);
};

#endif