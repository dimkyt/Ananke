#include "Renderer\shaders\DRshader_tonemap.h"
#include "Renderer\glsl.h"

// System includes
#include <stdlib.h>
#include <string>

DRshader_tonemap::DRshader_tonemap()
{
}

DRshader_tonemap::~DRshader_tonemap()
{
	glDeleteProgram(shaderID);
	glDeleteProgram(shader_logLumID);
	glDeleteBuffers(1, &logLuminance_FBO);
	glDeleteTextures(1, &logLuminance_map);
}

void DRshader_tonemap::init(unsigned width, unsigned height)
{
	win_width = width;
	win_height = height;
	maxMipmapLvl = (float)floor(log10((float)std::fmax(win_width, win_height))/log10(2.0));

	//Build shaders
	shaderID = LoadShaders(std::string("tonemap.vert").c_str(), std::string("tonemap.frag").c_str());
	uniform_sampler_color = glGetUniformLocation(shaderID, "tex_color");
	uniform_avgLogLuminance = glGetUniformLocation(shaderID, "avgLogLuminance");

	//Average log luminance shader
	shader_logLumID = LoadShaders(std::string("logLuminance.vert").c_str(), std::string("logLuminance.frag").c_str());
	uniform_sampler_color_logLum = glGetUniformLocation(shader_logLumID, "tex_color");

	//Create buffers and Textures
	glGenFramebuffers(1, &logLuminance_FBO);

	glGenTextures(1, &logLuminance_map);
	glBindTexture(GL_TEXTURE_2D, logLuminance_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, win_width, win_height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, 0);

	//Setup framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, logLuminance_FBO);
	
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
						    GL_TEXTURE_2D, logLuminance_map, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FRAMEBUFFER INCOMPLETE");
		return;
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DRshader_tonemap::calcAvgLogLuminance(GLuint render_map, GLuint screen_quad_vaoId)
{
	glViewport(0, 0, win_width, win_height);

	glBindFramebuffer(GL_FRAMEBUFFER, logLuminance_FBO);
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);

	glUseProgram(shader_logLumID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, render_map);
	glUniform1i(uniform_sampler_color_logLum, 0);

	glBindVertexArray(screen_quad_vaoId);
	glEnableVertexAttribArray(0);

	// Draw the screen quad
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glUseProgram(0);

	glBindTexture(GL_TEXTURE_2D, logLuminance_map);
	glGenerateMipmap(GL_TEXTURE_2D);
	glGetTexImage(GL_TEXTURE_2D, (GLint)maxMipmapLvl, GL_RED, GL_FLOAT, &avgLogLuminance);
	avgLogLuminance = exp(avgLogLuminance);
}

void DRshader_tonemap::tonemap(unsigned win_width, unsigned win_height, GLuint render_FBO, GLuint render_map, GLuint screen_quad_vaoId)
{
	//Get average log luminance
	calcAvgLogLuminance(render_map, screen_quad_vaoId);

	//Tonemap
	glBindFramebuffer(GL_FRAMEBUFFER, render_FBO);

	glUseProgram(shaderID);
	glUniform1f(uniform_avgLogLuminance, avgLogLuminance);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, render_map);
	glUniform1i(uniform_sampler_color, 0);

	glBindVertexArray(screen_quad_vaoId);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glUseProgram(0);
}