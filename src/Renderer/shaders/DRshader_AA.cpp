#include "Renderer\glsl.h"
#include "Renderer\shaders\DRshader_AA.h"

// System includes
#include <GL/glew.h>
#include <stdlib.h>
#include <string>

DRshader_AA::DRshader_AA()
{
}

DRshader_AA::~DRshader_AA()
{
	glDeleteProgram(shaderID);
}

void DRshader_AA::init()
{
	shaderID = LoadShaders(std::string("antiAlias.vert").c_str(), std::string("antiAlias.frag").c_str());
	uniform_pixel_size = glGetUniformLocation(shaderID, "pixel_size");
	uniform_weight = glGetUniformLocation(shaderID, "weight");
	uniform_sampler_normal = glGetUniformLocation(shaderID, "tex_normal");
	uniform_sampler_render = glGetUniformLocation(shaderID, "tex_render");
}

void DRshader_AA::AApass(unsigned win_width, unsigned win_height, GLuint normal_map,
						 GLuint render_FBO, GLuint render_map, GLuint screen_quad_vaoId)
{
	glViewport(0, 0, win_width, win_height);
	glBindFramebuffer(GL_FRAMEBUFFER, render_FBO);
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);

	glUseProgram(shaderID);

	glUniform2f(uniform_pixel_size, 1.f/float(win_width), 1.f/float(win_height));
	glUniform1f(uniform_weight, 1.f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, normal_map);
	glUniform1i(uniform_sampler_normal, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, render_map);
	glUniform1i(uniform_sampler_render, 1);

	// Draw the screen quad
	glBindVertexArray(screen_quad_vaoId);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glUseProgram(0);
}