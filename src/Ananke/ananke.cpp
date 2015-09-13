#include "lib3d\lib3D.h"
#include "Renderer\Renderer.h"

// System includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <GL\glew.h>
#include <GL\glut.h>

#define WINDOW_TITLE_PREFIX "Ananke"
#define PI 3.14159265358979323846f

using namespace std;

int CurrentWidth = 1920,
    CurrentHeight = 1080,
    WindowHandle = 0;

Mesh *scene;
Renderer* app_renderer;
 
void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void IdleFunction(void);
void mouseMotion(int x, int y);
void specialFunc(int key, int x, int y);
void keyPressed (unsigned char key, int x, int y);

int main(int argc, char* argv[])
{
  Initialize(argc, argv);

  scene = new Mesh(argv[1]);
  app_renderer = new Renderer(scene);
  app_renderer->init(CurrentWidth, CurrentHeight);

  glutMainLoop();
   
  exit(EXIT_SUCCESS);
}
 
void Initialize(int argc, char* argv[])
{
  GLenum GlewInitResult;

  InitWindow(argc, argv);

  glewExperimental = GL_TRUE;
  GlewInitResult = glewInit();
 
  if (GLEW_OK != GlewInitResult) {
      fprintf(
          stderr,
          "ERROR: %s\n",
          glewGetErrorString(GlewInitResult)
      );
      exit(EXIT_FAILURE);
  }
   
  fprintf(
      stdout,
      "INFO: OpenGL Version: %s\n",
      glGetString(GL_VERSION)
  );
 
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}
 
void InitWindow(int argc, char* argv[])
{
  glutInit(&argc, argv);
   
  //glutInitContextVersion(3, 3);
  //glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
  //glutInitContextProfile(GLUT_CORE_PROFILE);
  //
  //glutSetOption(
  //    GLUT_ACTION_ON_WINDOW_CLOSE,
  //    GLUT_ACTION_GLUTMAINLOOP_RETURNS
  //);
   
  glutInitWindowSize(CurrentWidth, CurrentHeight);
 
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
 
  WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);
 
  if(WindowHandle < 1) {
      fprintf(
          stderr,
          "ERROR: Could not create a new rendering window.\n"
      );
      exit(EXIT_FAILURE);
  }
 
  glutReshapeFunc(ResizeFunction);
  glutDisplayFunc(RenderFunction);
  glutPassiveMotionFunc(mouseMotion);
  glutSpecialFunc(specialFunc);
  glutKeyboardFunc(keyPressed);
  glutIdleFunc(IdleFunction);
}
 
void ResizeFunction(int Width, int Height)
{
  CurrentWidth = Width;
  CurrentHeight = Height;
  glViewport(0, 0, CurrentWidth, CurrentHeight);
}
 
void RenderFunction(void)
{
  app_renderer->display(app_renderer->render_mode);
 
  glutSwapBuffers();
  glutPostRedisplay();
}

void IdleFunction(void)
{
  glutPostRedisplay();
}


bool warped = false;
void mouseMotion(int x, int y) 
{	

  if(!warped) {
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    float diffx = width/2.0f - x;
    float diffy = height/2.0f - y;
    app_renderer->user.center.y -= (float)diffy*0.01f;
    app_renderer->user.center.x += (float)diffx*0.01f;
  
    app_renderer->user.dir = glm::vec3(
      cos(app_renderer->user.center.y) * sin(app_renderer->user.center.x),
      sin(app_renderer->user.center.y),
      cos(app_renderer->user.center.y) * cos(app_renderer->user.center.x)
                     );
  
    app_renderer->user.dir_strafe = glm::vec3(
      sin(app_renderer->user.center.x - 3.14f / 2.0f),
      0,
      cos(app_renderer->user.center.x - 3.14f / 2.0f)
    );
  
    app_renderer->user.up =
      glm::cross(-app_renderer->user.dir_strafe, app_renderer->user.dir);
  
    glutWarpPointer(width/2, height/2);
    warped = true;
  }
  else 
    warped = false;
  
  glutPostRedisplay();
}


float xrotrad, yrotrad;
void specialFunc(int key, int x, int y) {
  switch(key) {
    case GLUT_KEY_UP : app_renderer->user.eye += app_renderer->user.dir;
        break;
    case GLUT_KEY_DOWN : app_renderer->user.eye -= app_renderer->user.dir;
        break;
    case GLUT_KEY_LEFT :  app_renderer->user.eye += app_renderer->user.dir_strafe;
        break;
    case GLUT_KEY_RIGHT : app_renderer->user.eye -= app_renderer->user.dir_strafe;
        break;
    case GLUT_KEY_F12 : app_renderer->user.center = glm::vec3(0,10,0);
                        app_renderer->user.eye = glm::vec3(0,10,17.2);

                        app_renderer->user.dir = glm::vec3(
                          cos(app_renderer->user.center.y) * sin(app_renderer->user.center.x), 
                          sin(app_renderer->user.center.y),
                          cos(app_renderer->user.center.y) * cos(app_renderer->user.center.x)
                        );

                        app_renderer->user.dir_strafe = glm::vec3(
                          sin(app_renderer->user.center.x - 3.14f/2.0f),
                          0,
                          cos(app_renderer->user.center.x - 3.14f/2.0f)
                        );

                        app_renderer->user.up = glm::cross(-app_renderer->user.dir_strafe, app_renderer->user.dir);
        break;
    case GLUT_KEY_F1 : app_renderer->render_mode = RENDER;
        break;
    case GLUT_KEY_F2 : if(app_renderer->render_mode == RENDER)
                           app_renderer->render_mode = RENDER_NORMAL;
                       else
                       {
                           app_renderer->render_mode++;
                           app_renderer->render_mode %= RENDER_G_MAX;
                       }
        break;
    case GLUT_KEY_F3: app_renderer->switchAA();
        break;
    case GLUT_KEY_F4: app_renderer->switchBloom();
        break;
    case GLUT_KEY_F5: app_renderer->switchTonemapping();
        break;
    case GLUT_KEY_F6: app_renderer->switchSSAO();
        break;
  }
}

void keyPressed (unsigned char key, int x, int y) {
  switch(key)
  {
      case '.': app_renderer->lightPos.x += 0.2f;
          break;
      case ',': app_renderer->lightPos.x -= 0.2f;
          break;
      case 27: exit(0);	//ESC key
          break;
  }
} 