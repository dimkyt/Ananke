#ifndef _DR_SHADER_SSAO_
#define _DR_SHADER_SSAO_

#include <GL/glew.h>
#include <glm\glm.hpp>



#define KERNEL_SIZE 32
#define NOISE_SIZE 16
#define NOISE_WIDTH 4

class DRshader_SSAO
{
private:
	glm::vec3 kernel[KERNEL_SIZE];
	glm::vec3 noise[NOISE_SIZE];
	glm::vec2 noise_scale;

	GLuint SSAO_FBO,
		   tex_noise,
		   tex_occlusion,
		   tex_occlusion_blur;

	unsigned win_width,
			 win_height;

	float	width_inv,
			height_inv;

	GLuint shaderID,
		   shader_blurID,
		   uniform_width_inv,
		   uniform_width_inv_blur,
		   uniform_height_inv,
		   uniform_height_inv_blur,
		   uniform_P_inv,
		   uniform_P,
		   uniform_noise_scale,
		   uniform_kernel_size,
		   uniform_kernel,
		   uniform_radius,
		   uniform_sampler_depth,
		   uniform_sampler_normal,
		   uniform_sampler_noise,
		   uniform_sampler_SSAO;

public:
	DRshader_SSAO();
	~DRshader_SSAO();

	void init(unsigned width, unsigned height);
	void generateKernel();
	void generateNoise();
	void SSAOpass(glm::mat4 matrix_MVP_inv, glm::mat4 matrix_MVP, GLuint &depth_map, GLuint &normal_map, GLuint &screen_quad_vaoId);

	GLuint getSSAO() {return tex_occlusion_blur;}
};

#endif