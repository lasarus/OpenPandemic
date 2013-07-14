#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL_opengl.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#include "vertex.h"
#include "land.h"
#include "world_loader.h"
#include "sphere.h"

#ifndef PI
#define PI 3.14159265358979
#endif
 
int screen_width = 640, screen_height = 640, screen_bpp = 32;
int quit = 0;

SDL_Window * window = NULL;
SDL_GLContext window_context;

SDL_Event event;

float fov = 45.f;

int init_opengl()
{
  glewInit();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);

  glClearColor(0, 0, 0, 1);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, (GLfloat)screen_width / (GLfloat)screen_height, 0.01f, 100.0f);

  glMatrixMode(GL_MODELVIEW);

  if(glGetError() != GL_NO_ERROR)
    return 1;

  return 0;
}

int init()
{
  if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    return 1;
  
  window = SDL_CreateWindow("OpenPandemic 0.x", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_OPENGL);

  if(window == NULL)
    return 0;

  window_context = SDL_GL_CreateContext(window);

  if(init_opengl())
    return 0;
  
  return 0;
}

void update_time(Uint32 * ntime, Uint32 * ltime, Uint32 * dtime)
{
  *ntime = SDL_GetTicks();
  *dtime = *ntime - *ltime;
  *ltime = *ntime;
}

void load_world(const char * filename, landmass_t * landmass)
{
  FILE * fp;
  fp = fopen(filename, "r");

  load_landmass(landmass, fp);
  init_landmass(landmass);

  fclose(fp);
}

double camera_dist(float fov_r /* radians */, double sphere_r /* sphere radius */)
{
  fov_r /= 2;
  return sin(PI / 2 - fov_r) / tan(fov_r) + cos(PI / 2 - fov_r);
}

int main(int argc, char ** argv)
{
  landmass_t landmass;
  sphere_t sphere;
  Uint32 ntime, ltime = 0, dtime;
  double vangl = 0, hangl = 0, dist = 4;
  int mouse_x, mouse_y;
  s_vertex_t mouse_s;
  int wireframe = 0;
  int last_selected = -1;

  if(init())
    return 1;

  init_sphere(&sphere, 10, 20, 1);

  dist = camera_dist(fov / 180. * PI, 1.2);
  load_world("world.opw", &landmass);

  srand(time(NULL));

  while(!quit)
    {
      const Uint8 * keystate;
      vertex_t cameraLookAt, cameraUp, cameraPosition;
      int selected;
      while(SDL_PollEvent(&event))
	{
	  if(event.type == SDL_QUIT)
	    {
	      quit = 1;
	    }
	  else if(event.type == SDL_KEYDOWN)
	    {
	      SDL_Keycode key = event.key.keysym.sym;

	      if(key == SDLK_r)
		{
		  dist = camera_dist(fov / 180. * PI, 1.2);
		}
	      else if(key == SDLK_1)
		{
		  wireframe = !wireframe;
		}
	    }
	}

      update_time(&ntime, &ltime, &dtime);

      keystate = SDL_GetKeyboardState(NULL);
      SDL_GetMouseState(&mouse_x, &mouse_y);

      cameraPosition = new_vertex(cos(hangl) * dist * cos(vangl),
				  -sin(hangl) * dist * cos(vangl),
				  sin(vangl) * dist);
      cameraLookAt = new_vertex(0, 0, 0);
      cameraUp = new_vertex(0, 0, 1);
      sphere.r = 1.02;
      mouse_s = s_vertex_from_screen(&sphere, mouse_x, mouse_y, cameraLookAt, cameraPosition, cameraUp, fov, 0.1);

      if((selected = selected_country(&landmass, mouse_s)) > -1)
	{
	  if(selected != last_selected)
	    {
	      if(last_selected != -1)
		update_country(&landmass, last_selected, 1, 1, 1, 0);
	      update_country(&landmass, selected, 1, .5, .5, 0.01);
	    }
	}
      else if(last_selected != -1)
	{
	  update_country(&landmass, last_selected, 1, 1, 1, 0);
	}
      last_selected = selected;

      if(keystate[SDL_GetScancodeFromKey(SDLK_w)])
	{
	  vangl += 0.001 * dtime * sqrt(dist - 1.08);

	  if(vangl > PI / 2 - FLT_MIN)
	    vangl = PI / 2 - FLT_MIN;
	}
      else if(keystate[SDL_GetScancodeFromKey(SDLK_s)])
	{
	  vangl -= 0.001 * dtime * sqrt(dist - 1.08);

	  if(vangl < -PI / 2 + FLT_MIN)
	    vangl = -PI / 2 + FLT_MIN;
	}
      if(keystate[SDL_GetScancodeFromKey(SDLK_a)])
	hangl += 0.001 * dtime * sqrt(dist - 1.08);
      else if(keystate[SDL_GetScancodeFromKey(SDLK_d)])
	hangl -= 0.001 * dtime * sqrt(dist - 1.08);
      if(keystate[SDL_GetScancodeFromKey(SDLK_q)])
	{
	  dist -= (dist - 1.08) / 400 * dtime;
	  if(dist < 1.1)
	    dist = 1.1;
	}
      else if(keystate[SDL_GetScancodeFromKey(SDLK_e)])
	{
	  dist += (dist - 1) / 400 * dtime;
	  if(dist > 6)
	    dist = 6;
	}

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glLoadIdentity();

      gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
		cameraLookAt.x, cameraLookAt.y, cameraLookAt.z,
		cameraUp.x, cameraUp.y, cameraUp.z);

      if(wireframe)
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glColor3f(0, 0, 1);
      draw_sphere(&sphere);
      if(wireframe)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

      glColor3f(0, 1, 0);
      draw_landmass(&landmass);


      /* draw cursor */
      glPushMatrix();
      glColor3f(1, 0, 0);
      
      glRotatef(-mouse_s.sector * 180., 0, 0, 1);
      glRotatef(-mouse_s.ring * 90., 0, 1, 0);
      glTranslatef(1.02, 0, 0);

      glBegin(GL_TRIANGLES);
      
      glVertex3f(0, 0, -0.01);
      glVertex3f(0, -0.009, 0.01);
      glVertex3f(0, 0.009, 0.01);

      glEnd();

      glPopMatrix();

      /* */
      SDL_GL_SwapWindow(window);
    }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
