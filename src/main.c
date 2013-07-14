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
 
int screen_width = 640, screen_height = 480, screen_bpp = 32;
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
  if(init())
    return 1;

  init_sphere(&sphere, 20, 20, 1);

  dist = camera_dist(fov / 180. * PI, 1.2);
  load_world("world.opw", &landmass);

  printf("%f %f\n", dist, 1 / dist);

  srand(time(NULL));

  while(!quit)
    {
      const Uint8 * keystate;
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
	    }
	}

      update_time(&ntime, &ltime, &dtime);

      keystate = SDL_GetKeyboardState(NULL);

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
      gluLookAt(cos(hangl) * dist * cos(vangl), -sin(hangl) * dist * cos(vangl), sin(vangl) * dist,
		0, 0, 0,
		0, 0, 1);

      glColor3f(0, 0, 1);
      draw_sphere(&sphere);

      glColor3f(0, 1, 0);
      draw_landmass(&landmass);

      SDL_GL_SwapWindow(window);
    }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
