#ifndef _RENDERER_
#define _RENDERER_

#include "lib3d\lib3D.h"
#include "shaders\DRshaders.h"

// System includes
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

enum render_modes
{
	RENDER_NORMAL = 0,
	RENDER_DIFFUSE,
	RENDER_SPECULAR,
	RENDER_DEPTH,
	RENDER_SHADOWMAP,
	RENDER_BLOOM,
	RENDER_SSAO,
	RENDER_G_MAX,		//number of G buffer components
	RENDER				//normal render mode
};

struct User
{
	glm::vec3 eye;
	glm::vec3 center;
	glm::vec3 up;
	glm::vec3 dir;
	glm::vec3 dir_strafe;
	float speed;
	float miny;
	float maxy;
};

class Renderer
{
public:
	unsigned  win_width,
		win_height,
		render_mode;

	float FOV,
		plane_near,
		plane_far;

	glm::vec3 lightPos;			//light position in object space
	glm::vec3 lightPos_Proj;	//light position in view space
	glm::vec3 lightColor;
	glm::vec3 lightTgt;
	glm::vec3 lightUp;
	float light_FOV;


	Mesh* model;
	User user;

	Renderer(Mesh* m);
	~Renderer();

	void init(int width, int height);
	void createBuffers();
	void destroyBuffers();
	void calcMatrices();

	void setupShadowPass();
	void setupGeometryPass();
	void setupRenderPass();

	void shadowPass();
	void GeometryPass();
	void illuminationPass();
	void accPass();

	void displayComponent(GLenum render_target);
	void display(GLenum mode);

	void switchAA() { use_AA = !use_AA; }
	void switchBloom() { use_bloom = !use_bloom; }
	void switchTonemapping() { use_tonemapping = !use_tonemapping; }
	void switchSSAO() { use_SSAO = !use_SSAO; }


private:
	glm::mat4 matrix_M,
			  matrix_V,
			  matrix_P,
			  matrix_MV,
			  matrix_MVP,
			  matrix_MV_inv,
			  matrix_MV_inv_trans,
			  matrix_P_inv,
			  matrix_M_shadow,
			  matrix_V_shadow,
			  matrix_P_shadow,
			  matrix_MVP_shadow;

	//Geometry buffer
	GLuint geometry_FBO,
		   depth_map,
		   normal_map,
		   diffuse_map,
		   specular_map;

	//Shadow map
	GLuint shadow_FBO,
		   shadow_map;

	//Final image
	GLuint render_FBO,
		   render_map;

	//Screen quad buffers
	GLuint screen_quad_vaoId,
		   screen_quad_vboId;

	//Shaders
	DRshader_Gpass			shader_Gpass;
	DRshader_shadowmap		shader_shadowmap;
	DRshader_lightpass		shader_lightpass;
	DRshader_AA				shader_AA;
	DRshader_bloom			shader_bloom;
	DRshader_tonemap		shader_tonemap;
	DRshader_render			shader_render;
	DRshader_FXAA			shader_FXAA;
	DRshader_SSAO			shader_SSAO;
	DRshader_accPass		shader_accPass;

	bool use_AA,
		 use_bloom,
		 use_tonemapping,
		 use_SSAO;
};

#endif