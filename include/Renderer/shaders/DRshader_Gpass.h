#ifndef _DR_SHADER_GPASS_
#define _DR_SHADER_GPASS_


// System includes
#include <GL/glew.h>

class DRshader_Gpass
{
public:
	GLuint shaderID,
		   uniform_MVP,
		   uniform_MV_inv_trans,
		   uniform_diffuseMat,
		   uniform_specularMat,
		   uniform_Ns,
		   uniform_has_diffuse_tex,
		   uniform_has_specular_tex,
		   uniform_sampler_diffuse,
		   uniform_sampler_specular;

	DRshader_Gpass();
	~DRshader_Gpass();
	void init();
};

#endif