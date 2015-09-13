#ifndef _DR_SHADER_SHADOWMAP_
#define _DR_SHADER_SHADOWMAP_

#include <GL/glew.h>

class DRshader_shadowmap
{
public:
	GLuint shaderID,
		   uniform_MVP;

	DRshader_shadowmap();
	~DRshader_shadowmap();
	void init();
};

#endif