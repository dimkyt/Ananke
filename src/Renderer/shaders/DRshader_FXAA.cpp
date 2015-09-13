#include "Renderer\shaders\DRshader_FXAA.h"
#include "Renderer\glsl.h"

// System includes
#include <stdlib.h>
#include <string>

DRshader_FXAA::DRshader_FXAA()
{
}

DRshader_FXAA::~DRshader_FXAA()
{
	glDeleteProgram(shaderID);
}

void DRshader_FXAA::init()
{
	shaderID = LoadShaders(std::string("FXAA.vert").c_str(), std::string("FXAA.frag").c_str());
	uniform_pixel_size = glGetUniformLocation(shaderID, "pixel_size");
	uniform_sampler_color = glGetUniformLocation(shaderID, "tex_color");
}

void DRshader_FXAA::FXAApass(unsigned win_width, unsigned win_height,
				   GLuint render_FBO, GLuint render_map, GLuint screen_quad_vaoId)
{
	glViewport(0, 0, win_width, win_height);
	glBindFramebuffer(GL_FRAMEBUFFER, render_FBO);
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);

	glUseProgram(shaderID);

	glUniform2f(uniform_pixel_size, 1.f/float(win_width), 1.f/float(win_height));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, render_map);
	glUniform1i(uniform_sampler_color, 0);

	// Draw the screen quad
	glBindVertexArray(screen_quad_vaoId);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glUseProgram(0);
}