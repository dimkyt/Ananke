#include "Renderer\shaders\DRshader_bloom.h"
#include "Renderer\glsl.h"

// System includes
#include <stdlib.h>
#include <string>

DRshader_bloom::DRshader_bloom()
{
}

DRshader_bloom::~DRshader_bloom()
{
	glDeleteProgram(shaderID);
	glDeleteBuffers(1, &bloom_FBO);
	glDeleteTextures(1, &bloom_map_x);
	glDeleteTextures(1, &bloom_map_y);
}

void DRshader_bloom::init(unsigned width, unsigned height)
{
	win_width = width;
	win_height = height;

	bloom_width = width/4;
	bloom_height = height/4;

	//Build shaders
	shaderID = LoadShaders(std::string("bloom.vert").c_str(), std::string("bloom.frag").c_str());
	uniform_axis = glGetUniformLocation(shaderID, "axis");
	uniform_sampler_in = glGetUniformLocation(shaderID, "tex_in");
	uniform_sampler_color = glGetUniformLocation(shaderID, "tex_color");

	//Create buffers and textures
	glGenFramebuffers(1, &bloom_FBO);

	//setup bloom map horizontal
	glGenTextures(1, &bloom_map_x);
	glBindTexture(GL_TEXTURE_2D, bloom_map_x);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, bloom_width, bloom_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//setup bloom map vertical
	glGenTextures(1, &bloom_map_y);
	glBindTexture(GL_TEXTURE_2D, bloom_map_y);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, bloom_width, bloom_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, 0);

	//Setup framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, bloom_FBO);
	
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							GL_TEXTURE_2D, bloom_map_x, 0);
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
							GL_TEXTURE_2D, bloom_map_y, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FRAMEBUFFER INCOMPLETE\n");
		return;
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DRshader_bloom::bloomPass(GLuint render_map, GLuint screen_quad_vaoId)
{
	glViewport(0, 0, bloom_width, bloom_height);
	glBindFramebuffer(GL_FRAMEBUFFER, bloom_FBO);

	//Horizontal pass
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);

	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(shaderID);

	glUniform2f(uniform_axis, 1.f, .0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, render_map);
	glUniform1i(uniform_sampler_in, 0);

	// Draw the screen quad
	glBindVertexArray(screen_quad_vaoId);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glUseProgram(0);


	
	//Vertical pass
	buffers[0] = GL_COLOR_ATTACHMENT1;
	glDrawBuffers(1, buffers);

	glUseProgram(shaderID);

	glUniform2f(uniform_axis, .0f, 1.f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bloom_map_x);
	glUniform1i(uniform_sampler_in, 0);

	//Draw the screen quad
	glBindVertexArray(screen_quad_vaoId);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glUseProgram(0);
}