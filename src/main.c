#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL_opengl.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#ifndef PI
#define PI 3.14159265358979
#endif
 
int screen_width = 640, screen_height = 480, screen_bpp = 32;
int quit = 0;

SDL_Window * window = NULL;
SDL_GLContext window_context;

SDL_Event event;

int init_opengl()
{
  glewInit();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);

  glClearColor(0, 0, 1, 1);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)screen_width / (GLfloat)screen_height, 0.1f, 100.0f);

  glMatrixMode(GL_MODELVIEW);

  if(glGetError() != GL_NO_ERROR)
    return 1;

  return 0;
}

int init()
{
  if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    return 1;
  
  window = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_OPENGL);

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

GLuint sphere_buffer = 0;
typedef struct vertex
{
  float x, y, z;
} vertex_t;

typedef struct rect
{
  vertex_t v[4];
} rect_t;

const int sphere_rings = 50;
const int sphere_sectors = 50;

void init_sphere()
{
  rect_t * sphere;
  int i, j;

  sphere = malloc(sizeof(rect_t) * sphere_rings
		  * sphere_sectors);

  for(i = 0; i < sphere_sectors; i++)
    {
      double sector_angl, prev_sector_angl;

      sector_angl = (i / (double)sphere_sectors) * PI * 2;
      prev_sector_angl = ((i - 1) / (double)sphere_sectors) * PI * 2;
      for(j = 1; j < sphere_rings + 1; j++)
	{
	  double ring_angl, prev_ring_angl;

	  ring_angl = (j / (double)sphere_rings - 0.5) * PI;
	  prev_ring_angl = ((j - 1) / (double)sphere_rings - 0.5) * PI;

	  sphere[(j - 1) + i * sphere_rings].v[0].x = cos(prev_sector_angl) * cos(prev_ring_angl);
	  sphere[(j - 1) + i * sphere_rings].v[0].y = -sin(prev_sector_angl) * cos(prev_ring_angl);
	  sphere[(j - 1) + i * sphere_rings].v[0].z = sin(prev_ring_angl);

	  sphere[(j - 1) + i * sphere_rings].v[1].x = cos(prev_sector_angl) * cos(ring_angl);
	  sphere[(j - 1) + i * sphere_rings].v[1].y = -sin(prev_sector_angl) * cos(ring_angl);
	  sphere[(j - 1) + i * sphere_rings].v[1].z = sin(ring_angl);

	  sphere[(j - 1) + i * sphere_rings].v[2].x = cos(sector_angl) * cos(ring_angl);
	  sphere[(j - 1) + i * sphere_rings].v[2].y = -sin(sector_angl) * cos(ring_angl);
	  sphere[(j - 1) + i * sphere_rings].v[2].z = sin(ring_angl);

	  sphere[(j - 1) + i * sphere_rings].v[3].x = cos(sector_angl) * cos(prev_ring_angl);
	  sphere[(j - 1) + i * sphere_rings].v[3].y = -sin(sector_angl) * cos(prev_ring_angl);
	  sphere[(j - 1) + i * sphere_rings].v[3].z = sin(prev_ring_angl);
	}
    }

  glGenBuffers(1, &sphere_buffer);

  glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(rect_t) * sphere_rings* sphere_sectors,
	       sphere, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  free(sphere);
}

void draw_sphere()
{
  glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
  glVertexPointer(3, GL_FLOAT, 0, NULL);  
  glEnableClientState(GL_VERTEX_ARRAY);

  glDrawArrays(GL_QUADS, 0, sphere_rings * sphere_sectors * 4);

  glDisableClientState(GL_VERTEX_ARRAY);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main(int argc, char ** argv)
{
  Uint32 ntime, ltime = 0, dtime;
  double vangl = 0, hangl = 0, dist = 4;
  if(init())
    return 1;

  init_sphere();

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
	}

      update_time(&ntime, &ltime, &dtime);

      keystate = SDL_GetKeyboardState(NULL);

      if(keystate[SDL_GetScancodeFromKey(SDLK_w)])
	{
	  vangl += 0.001 * dtime;

	  if(vangl > PI / 2 - 0.1)
	    vangl = PI / 2 - 0.1;
	}
      else if(keystate[SDL_GetScancodeFromKey(SDLK_s)])
	{
	  vangl -= 0.001 * dtime;

	  if(vangl < 0.1)
	    vangl = 0.1;
	}
      if(keystate[SDL_GetScancodeFromKey(SDLK_a)])
	hangl += 0.001 * dtime;
      else if(keystate[SDL_GetScancodeFromKey(SDLK_d)])
	hangl -= 0.001 * dtime;
      if(keystate[SDL_GetScancodeFromKey(SDLK_q)])
	dist += 0.01 * dtime;
      else if(keystate[SDL_GetScancodeFromKey(SDLK_e)])
	dist -= 0.01 * dtime;

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glLoadIdentity();
      gluLookAt(cos(hangl) * dist * cos(vangl), -sin(hangl) * dist * cos(vangl), sin(vangl) * dist,
		0, 0, 0,
		0, 0, 1);

      glColor3f(0, 1, 0);
      draw_sphere();

      SDL_GL_SwapWindow(window);
    }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
