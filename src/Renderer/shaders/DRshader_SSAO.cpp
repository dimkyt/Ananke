#include "Renderer\shaders\DRshader_SSAO.h"
#include "Renderer\glsl.h"


// System includes
#include <stdlib.h>
#include <string>
#include <time.h>

#define RADIUS 3.f

DRshader_SSAO::DRshader_SSAO()
{
}

DRshader_SSAO::~DRshader_SSAO()
{
	glDeleteProgram(shaderID);
	glDeleteProgram(shader_blurID);
	glDeleteBuffers(1, &SSAO_FBO);
	glDeleteTextures(1, &tex_noise);
	glDeleteTextures(1, &tex_occlusion);
}

void DRshader_SSAO::init(unsigned width, unsigned height)
{
	win_width = width;
	win_height = height;
	width_inv = 1.0f/(float)win_width;
	height_inv = 1.0f/(float)win_height;
	noise_scale.x = (float)win_width/NOISE_WIDTH;
	noise_scale.y = (float)win_height / NOISE_WIDTH;
	generateKernel();
	generateNoise();

	//Build shaders
	shaderID = LoadShaders(std::string("SSAO.vert").c_str(), std::string("SSAO.frag").c_str());
	uniform_width_inv = glGetUniformLocation(shaderID, "width_inv");
	uniform_height_inv = glGetUniformLocation(shaderID, "height_inv");
	uniform_P_inv = glGetUniformLocation(shaderID, "P_inv");
	uniform_P = glGetUniformLocation(shaderID, "matrix_P");
	uniform_noise_scale = glGetUniformLocation(shaderID, "noise_scale");
	uniform_kernel_size = glGetUniformLocation(shaderID, "kernel_size");
	uniform_kernel = glGetUniformLocation(shaderID, "kernel");
	uniform_radius = glGetUniformLocation(shaderID, "radius");
	uniform_sampler_depth = glGetUniformLocation(shaderID, "tex_depth");
	uniform_sampler_normal = glGetUniformLocation(shaderID, "tex_normal");
	uniform_sampler_noise = glGetUniformLocation(shaderID, "tex_noise");

	//Blur shader
	shader_blurID = LoadShaders(std::string("SSAO_blur.vert").c_str(), std::string("SSAO_blur.frag").c_str());
	uniform_width_inv_blur = glGetUniformLocation(shader_blurID, "width_inv");
	uniform_height_inv_blur = glGetUniformLocation(shader_blurID, "height_inv");
	uniform_sampler_SSAO = glGetUniformLocation(shader_blurID, "tex_SSAO");

	//Create buffers and textures
	glGenFramebuffers(1, &SSAO_FBO);

	//setup noise texture
	glGenTextures(1, &tex_noise);
	glBindTexture(GL_TEXTURE_2D, tex_noise);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, NOISE_WIDTH, NOISE_WIDTH, 0, GL_RGB, GL_FLOAT, noise);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//setup occlusion texture
	glGenTextures(1, &tex_occlusion);
	glBindTexture(GL_TEXTURE_2D, tex_occlusion);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, win_width, win_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//setup blurred occlusion texture
	glGenTextures(1, &tex_occlusion_blur);
	glBindTexture(GL_TEXTURE_2D, tex_occlusion_blur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, win_width, win_height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, 0);


	//Setup framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, SSAO_FBO);

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							GL_TEXTURE_2D, tex_occlusion, 0);
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
							GL_TEXTURE_2D, tex_occlusion_blur, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FRAMEBUFFER INCOMPLETE\n");
		return;
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DRshader_SSAO::generateKernel()
{
	float scale;
	srand((unsigned)time(0));

	for(int i=0; i<KERNEL_SIZE; i++)
	{
		kernel[i].x = (float)rand()/((float)RAND_MAX*0.5f) - 1.0f;
		kernel[i].y = (float)rand()/((float)RAND_MAX*0.5f) - 1.0f;
		kernel[i].z = (float)rand()/(float)RAND_MAX;

		kernel[i] = glm::normalize(kernel[i]);
		kernel[i] *= (float)rand()/(float)RAND_MAX;

		scale = float(i) / float(KERNEL_SIZE);
		scale = glm::mix(0.1f, 1.0f, scale*scale);
		kernel[i] *= scale;
	}
}

void DRshader_SSAO::generateNoise()
{
	srand((unsigned)time(0));

	for(int i=0; i<NOISE_SIZE; i++)
	{
		noise[i].x = (float)rand()/((float)RAND_MAX*0.5f) - 1.0f;
		noise[i].y = (float)rand()/((float)RAND_MAX*0.5f) - 1.0f;
		noise[i].z = .0;

		noise[i] = glm::normalize(noise[i]);
	}
}

void DRshader_SSAO::SSAOpass(glm::mat4 matrix_P_inv, glm::mat4 matrix_P, GLuint &depth_map, GLuint &normal_map, GLuint &screen_quad_vaoId)
{
	glViewport(0, 0, win_width, win_height);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAO_FBO);
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderID);

	glUniform1f(uniform_width_inv, width_inv);
	glUniform1f(uniform_height_inv, height_inv);
	glUniformMatrix4fv(uniform_P_inv, 1, GL_FALSE, &matrix_P_inv[0][0]);
	glUniformMatrix4fv(uniform_P, 1, GL_FALSE, &matrix_P[0][0]);
	glUniform2f(uniform_noise_scale, noise_scale.x, noise_scale.y);
	glUniform1f(uniform_kernel_size, (float)KERNEL_SIZE);
	glUniform3fv(uniform_kernel, KERNEL_SIZE, &kernel[0].x);
	glUniform1f(uniform_radius, RADIUS);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glUniform1i(uniform_sampler_depth, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal_map);
	glUniform1i(uniform_sampler_normal, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex_noise);
	glUniform1i(uniform_sampler_noise, 2);

	// Draw the screen quad
	glBindVertexArray(screen_quad_vaoId);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glUseProgram(0);


	//Blur shader
	buffers[0] = GL_COLOR_ATTACHMENT1;
	glDrawBuffers(1, buffers);

	glUseProgram(shader_blurID);
	glUniform1f(uniform_width_inv_blur, width_inv);
	glUniform1f(uniform_height_inv_blur, height_inv);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_occlusion);
	glUniform1i(uniform_sampler_SSAO, 0);

	// Draw the screen quad
	glBindVertexArray(screen_quad_vaoId);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glUseProgram(0);
}