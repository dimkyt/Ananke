#ifndef _DR_SHADER_BLOOM_
#define _DR_SHADER_BLOOM_


// System includes
#include <GL/glew.h>

class DRshader_bloom
{
private:
	GLuint bloom_FBO,
		     bloom_map_x,
		     bloom_map_y;

	unsigned win_width,
			     win_height,
			     bloom_width,
			     bloom_height;

	GLuint shaderID,
		     uniform_axis,
		     uniform_sampler_in,
		     uniform_sampler_color;

public:
	DRshader_bloom();
	~DRshader_bloom();

	void init(unsigned width, unsigned height);
	void bloomPass(GLuint render_map, GLuint screen_quad_vaoId);

	GLuint getBloomMap() {return bloom_map_y;}
};

#endif