#include "Renderer\Renderer.h"

#include <GL\glew.h>
#include <GL\glut.h>

#define SHADOW_MAP_WIDTH 2048
#define SHADOW_MAP_HEIGHT 2048

static const GLfloat screen_quad_data[] =
{
  -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
  -1.0f,  1.0f, 0.0f,
  -1.0f,  1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   1.0f,  1.0f, 0.0f
};

Renderer::Renderer(Mesh* m)
{
  model = m;

  user.eye = glm::vec3(.0, 10.0, 27.2);
  user.center = glm::vec3(.0, .1, .0);
  user.up = glm::vec3(.0, 1.0, .0);
  user.dir = glm::vec3(.0, .1, 1000.);

  FOV = 45.;
  win_width = 800;
  win_height = 600;
  plane_near = 1.;
  plane_far = 400.;

  //cornel
  /*
  lightPos = glm::vec3(0.0, 19.0, 0.0);
  lightColor = glm::vec3(1., 1., 1.);
  lightTgt = glm::vec3(.0, 1., .0);
  lightUp = glm::vec3(1., .0, .0);
  light_FOV = 130.0f;
  */

  // SPONZA:
  /*
  lightPos = glm::vec3(0.0, 50.0, 10.0);
  lightColor = glm::vec3(1., 1., 1.);
  lighTgt = glm::vec3(0,10,0);
  lightUp = glm::vec3(1.0, 0.0, 0.0);
  light_FOV = 130.0f;
  */

  //ARENA
  
  lightPos = glm::vec3(0.0, 50.0, 10.0);
  lightColor = glm::vec3(1., 1., 1.);
  lightTgt = glm::vec3(0,10,0);
  lightUp = glm::vec3(0.0, 1.0, 0.0);
  light_FOV = 130.0f;
  

  render_mode = RENDER;
}

Renderer::~Renderer()
{
  destroyBuffers();
}

void Renderer::init(int width, int height)
{
  win_width = width;
  win_height = height;
  use_AA = false;
  use_bloom = false;
  use_tonemapping = false;
  use_SSAO = false;

  //Setup render targets
  createBuffers();
  setupShadowPass();
  setupGeometryPass();
  setupRenderPass();

  //Build shaders
  shader_Gpass.init();
  shader_shadowmap.init();
  shader_lightpass.init();
  shader_AA.init();
  shader_FXAA.init();
  shader_bloom.init(win_width, win_height);
  shader_tonemap.init(win_width, win_height);
  shader_SSAO.init(win_width, win_height);
  shader_accPass.init();
  shader_render.init();
}

void Renderer::createBuffers()
{
  glGenFramebuffersEXT(1, &geometry_FBO);
  glGenFramebuffersEXT(1, &shadow_FBO);
  glGenFramebuffersEXT(1, &render_FBO);

  //setup depth map
  glGenTextures(1, &depth_map);
    glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, win_width, win_height, 0, 
          GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  
  //setup normal buffer
  glGenTextures(1, &normal_map);
  glBindTexture(GL_TEXTURE_2D, normal_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win_width, win_height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  
  //setup diffuse buffer
  glGenTextures(1, &diffuse_map);
  glBindTexture(GL_TEXTURE_2D, diffuse_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win_width, win_height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  
  //setup specular buffer
  glGenTextures(1, &specular_map);
  glBindTexture(GL_TEXTURE_2D, specular_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win_width, win_height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  //setup shadowmap
  glGenTextures(1, &shadow_map);
    glBindTexture(GL_TEXTURE_2D, shadow_map);
  glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, 
          GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  //setup render map
  glGenTextures(1, &render_map);
  glBindTexture(GL_TEXTURE_2D, render_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win_width, win_height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  
  glBindTexture(GL_TEXTURE_2D, 0);

  //SCREEN QUAD
  glGenVertexArrays(1, &screen_quad_vaoId);
  glBindVertexArray(screen_quad_vaoId);
  glGenBuffers(1, &screen_quad_vboId);
  glBindBuffer(GL_ARRAY_BUFFER, screen_quad_vboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad_data), screen_quad_data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Renderer::destroyBuffers()
{
}

void Renderer::setupGeometryPass()
{
  glBindFramebuffer(GL_FRAMEBUFFER, geometry_FBO);
  
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
              GL_TEXTURE_2D, depth_map, 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
              GL_TEXTURE_2D, normal_map, 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
              GL_TEXTURE_2D, diffuse_map, 0);
  glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
              GL_TEXTURE_2D, specular_map, 0);

  if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    printf("FRAMEBUFFER INCOMPLETE");
    return;
  }
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::setupShadowPass()
{
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_FBO);
  
  glFramebufferTexture2DEXT( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
              GL_TEXTURE_2D, shadow_map, 0);

  if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    printf("FRAMEBUFFER INCOMPLETE");
    return;
  }
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::setupRenderPass()
{
  glBindFramebuffer(GL_FRAMEBUFFER, render_FBO);
  
  glFramebufferTexture2DEXT( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
              GL_TEXTURE_2D, render_map, 0);

  if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    printf("FRAMEBUFFER INCOMPLETE");
    return;
  }
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::calcMatrices()
{
  matrix_M = glm::mat4(1.0f);
  matrix_V = glm::lookAt(user.eye, user.eye+user.dir, user.up);
  matrix_P = glm::perspective(FOV, (float)win_width/(float)win_height, plane_near, plane_far);
  matrix_MV = matrix_V*matrix_M;
  matrix_MVP = matrix_P*matrix_MV;
  
  matrix_MV_inv = glm::inverse(matrix_MV);
  matrix_MV_inv_trans = glm::transpose(matrix_MV_inv);
  matrix_P_inv = glm::inverse(matrix_P);

  //Transform light position from object to view space
  lightPos_Proj = (matrix_MV*glm::vec4(lightPos, 1.0)).swizzle(glm::X, glm::Y, glm::Z);

  //Shadowmap matrices
  matrix_M_shadow = glm::mat4(1.0f);
  matrix_V_shadow = glm::lookAt(lightPos, lightTgt, lightUp);
  matrix_P_shadow = glm::perspective(light_FOV, (float)SHADOW_MAP_WIDTH/(float)SHADOW_MAP_HEIGHT, plane_near, plane_far);
  matrix_MVP_shadow = matrix_P_shadow*matrix_V_shadow*matrix_M_shadow;
}

void Renderer::shadowPass()
{
  glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_FBO);

  glClearDepth(1);
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);

  glUseProgram(shader_shadowmap.shaderID);
  glUniformMatrix4fv(shader_shadowmap.uniform_MVP, 1, GL_FALSE, &matrix_MVP_shadow[0][0]);

  glBindVertexArray(model->vaoId);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->iboId);

  unsigned int idxOffset = 0;
  for(unsigned int i=0; i<model->numGroups; i++)
  {
    glDrawElements(GL_TRIANGLES, model->groups[i].vertIdx.size(), GL_UNSIGNED_INT, (void*)idxOffset);
    idxOffset += model->groups[i].vertIdx.size()*sizeof(unsigned int);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);

  glUseProgram(0);
}

void Renderer::GeometryPass()
{
  glViewport(0, 0, win_width, win_height);
  glBindFramebuffer(GL_FRAMEBUFFER, geometry_FBO);
  GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
  glDrawBuffers(3, buffers);

  glClearColor(0,0,0,1);
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glColorMask(1,1,1,1);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);

  glUseProgram(shader_Gpass.shaderID);
  glUniformMatrix4fv(shader_Gpass.uniform_MVP, 1, GL_FALSE, &matrix_MVP[0][0]);
  glUniformMatrix4fv(shader_Gpass.uniform_MV_inv_trans, 1, GL_FALSE, &matrix_MV_inv_trans[0][0]);

  glBindVertexArray(model->vaoId);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->iboId);

  unsigned int idxOffset = 0;
  for(unsigned int i=0; i<model->numGroups; i++)
  {
    //set diffuse material
    glm::vec3 Kd = model->materials[model->groups[i].matIdx].diffuse;
    glUniform3f(shader_Gpass.uniform_diffuseMat, Kd.x, Kd.y, Kd.z);

    //set specular material
    glm::vec3 Ks = model->materials[model->groups[i].matIdx].specular;
    glUniform3f(shader_Gpass.uniform_specularMat, Ks.x, Ks.y, Ks.z);

    //check for diffuse texture
    float has_diffuse_tex = model->materials[model->groups[i].matIdx].has_texture[MAP_DIFFUSE];
    glUniform1f(shader_Gpass.uniform_has_diffuse_tex, has_diffuse_tex);

    //check for specular texture
    float has_specular_tex = model->materials[model->groups[i].matIdx].has_texture[MAP_SPECULAR];
    float Ns = model->materials[model->groups[i].matIdx].Ns;
    glUniform1f(shader_Gpass.uniform_has_specular_tex, has_specular_tex);
    glUniform1f(shader_Gpass.uniform_Ns, Ns);

    if(has_diffuse_tex == 1.0)
    {
      glActiveTexture(GL_TEXTURE0 + MAP_DIFFUSE);
      unsigned int diffuse_texId = model->materials[model->groups[i].matIdx].textureID[MAP_DIFFUSE];
      glBindTexture(GL_TEXTURE_2D, diffuse_texId);
      glUniform1i(shader_Gpass.uniform_sampler_diffuse, MAP_DIFFUSE);
    }
    if(has_specular_tex == 1.0)
    {
      glActiveTexture(GL_TEXTURE0 + MAP_SPECULAR);
      unsigned int specular_texId = model->materials[model->groups[i].matIdx].textureID[MAP_SPECULAR];
      glBindTexture(GL_TEXTURE_2D, specular_texId);
      glUniform1i(shader_Gpass.uniform_sampler_specular, MAP_SPECULAR);
    }

    glDrawElements(GL_TRIANGLES, model->groups[i].vertIdx.size(), GL_UNSIGNED_INT, (void*)idxOffset);
    idxOffset += model->groups[i].vertIdx.size()*sizeof(unsigned int);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);

  glBindVertexArray(0);

  glUseProgram(0);
}

void Renderer::illuminationPass()
{
  glViewport(0, 0, win_width, win_height);
  glBindFramebuffer(GL_FRAMEBUFFER, render_FBO);
  GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, buffers);

  glClearDepth(1);
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  glUseProgram(shader_lightpass.shaderID);
  glUniform1f(shader_lightpass.uniform_width_inv, 1.0f/(float)win_width);
  glUniform1f(shader_lightpass.uniform_height_inv, 1.0f/(float)win_height);
  glUniform3f(shader_lightpass.uniform_lightPos, lightPos_Proj.x, lightPos_Proj.y, lightPos_Proj.z);
  glUniform3f(shader_lightpass.uniform_lightColor, lightColor.x, lightColor.y, lightColor.z);
  glUniformMatrix4fv(shader_lightpass.uniform_MVP_shadow, 1, GL_FALSE, &matrix_MVP_shadow[0][0]);
  glUniformMatrix4fv(shader_lightpass.uniform_P_inv, 1, GL_FALSE, &matrix_P_inv[0][0]);
  glUniformMatrix4fv(shader_lightpass.uniform_MV_inv, 1, GL_FALSE, &matrix_MV_inv[0][0]);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, normal_map);
  glUniform1i(shader_lightpass.uniform_sampler_normal, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, diffuse_map);
  glUniform1i(shader_lightpass.uniform_sampler_diffuse, 1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, specular_map);
  glUniform1i(shader_lightpass.uniform_sampler_specular, 2);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glUniform1i(shader_lightpass.uniform_sampler_depth, 3);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, shadow_map);
  glUniform1i(shader_lightpass.uniform_sampler_shadow, 4);

  // Draw the screen quad
  glBindVertexArray(screen_quad_vaoId);
  glEnableVertexAttribArray(0);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
  glUseProgram(0);
}

void Renderer::accPass()
{
  glViewport(0, 0, win_width, win_height);
  glBindFramebuffer(GL_FRAMEBUFFER, render_FBO);
  GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, buffers);

  glUseProgram(shader_accPass.shaderID);
  glUniform1f(shader_accPass.uniform_use_SSAO, use_SSAO);
  glUniform1f(shader_accPass.uniform_use_bloom, use_bloom);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, render_map);
  glUniform1i(shader_render.uniform_sampler_color, 0);

  if(use_SSAO)
  {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shader_SSAO.getSSAO());
    glUniform1i(shader_accPass.uniform_sampler_SSAO, 1);
  }
  if(use_bloom)
  {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, shader_bloom.getBloomMap());
    glUniform1i(shader_accPass.uniform_sampler_bloom, 2);
  }

  glBindVertexArray(screen_quad_vaoId);
  glEnableVertexAttribArray(0);

  // Draw the screen quad
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glDisableVertexAttribArray(0);
  glBindVertexArray(0);

  glUseProgram(0);
}

void Renderer::displayComponent(GLenum render_target)
{
  glViewport(0, 0, win_width, win_height);
  GLuint component;
  switch(render_target)
  {
    case RENDER_NORMAL : component = normal_map;
      break;
    case RENDER_DIFFUSE : component = diffuse_map;
      break;
    case RENDER_SPECULAR : component = specular_map;
      break;
    case RENDER_DEPTH : component = depth_map;
      break;
    case RENDER_SHADOWMAP : component = shadow_map;
      break;
    case RENDER_BLOOM : component = shader_bloom.getBloomMap();
      break;
    case RENDER_SSAO : component = shader_SSAO.getSSAO();
      break;
    case RENDER : component = render_map;
      break;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glClearDepth(1);
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  glUseProgram(shader_render.shaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, component);
  glUniform1i(shader_render.uniform_sampler_color, 0);

  glBindVertexArray(screen_quad_vaoId);
  glEnableVertexAttribArray(0);

  // Draw the screen quad
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glDisableVertexAttribArray(0);
  glBindVertexArray(0);

  glUseProgram(0);
}

void Renderer::display(GLenum mode)
{
  calcMatrices();
  GeometryPass();

  shadowPass();
  illuminationPass();
  if(use_SSAO)
    shader_SSAO.SSAOpass(matrix_P_inv, matrix_P, depth_map, normal_map, screen_quad_vaoId);
  if(use_bloom)
    shader_bloom.bloomPass(render_map, screen_quad_vaoId);
  if(use_tonemapping)
    shader_tonemap.tonemap(win_width, win_height, render_FBO, render_map, screen_quad_vaoId);
  if(use_AA)
    //shader_AA.AApass(win_width, win_height, normal_map, render_FBO, render_map, screen_quad_vaoId);
    shader_FXAA.FXAApass(win_width, win_height, render_FBO, render_map, screen_quad_vaoId);

  accPass();

  displayComponent(mode);
}