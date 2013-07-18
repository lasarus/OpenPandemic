#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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
#include "font.h"
#include "texture.h"
#include "disease.h"

#ifndef PI
#define PI 3.14159265358979
#endif
 
int screen_width = 640, screen_height = 480, screen_bpp = 32;
int quit = 0;

SDL_Window * window = NULL;
SDL_GLContext window_context;

SDL_Event event;

float fov = 45.f;

GLuint font;

GLfloat projectionv[16];
GLfloat orthov[16];

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
  glOrtho(0, screen_width, screen_height, 0, -1, 1);
  glGetFloatv(GL_PROJECTION_MATRIX, orthov);

  glLoadIdentity();
  gluPerspective(fov, (GLfloat)screen_width / (GLfloat)screen_height, 0.01f, 100.0f);
  glGetFloatv(GL_PROJECTION_MATRIX, projectionv);

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
    return 1;

  window_context = SDL_GL_CreateContext(window);

  if(init_opengl())
    return 1;
  
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

void load_font_texture(const char * path)
{
  SDL_Surface * tmp = NULL;
  tmp = load_surface(path);

  if(tmp == NULL)
    {
      return;
    }

  glGenTextures(1, &font);
  load_texture(tmp, font);
  SDL_FreeSurface(tmp);
}

int main(int argc, char ** argv)
{
  landmass_t landmass;
  sphere_t sphere;
  disease_simulation_t simulation;
  Uint32 ntime, ltime = 0, dtime;
  double vangl = 0, hangl = 0, dist = 4;
  double camera_speed = 1;
  int mouse_x, mouse_y;
  s_vertex_t mouse_s;
  int mouse_outside;
  int wireframe = 0;
  int last_selected = -1;

  Uint32 last_step = 0;
  Uint32 fps_starttick, fps_start_frame = 0, frame = 0;
  float fps = 0;

  font_buffer_t tooltip;
  int tooltip_activated = 0;
  font_buffer_t fps_fb;

  if(init())
    return 1;

  init_sphere(&sphere, 10, 20, 1);

  dist = camera_dist(fov / 180. * PI, 1.2);

  printf("loading %s ...\n", DATADIR "/world.opw");
  load_world(DATADIR "/world.opw", &landmass);
  printf("done!\n");

  srand(time(NULL));

  printf("loading %s ...\n", DATADIR "/font.png");
  load_font_texture(DATADIR "/font.png");
  printf("done!\n");
  
  tooltip = generate_font_buffer_vbo(font, "");
  fps_fb = generate_font_buffer_vbo(font, "fps: 0");
  glBindTexture(GL_TEXTURE_2D, 0);

  init_disease_simulation(&simulation, new_disease(DISEASE_VIRUS));

  last_step = fps_starttick = SDL_GetTicks();

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

      if(ntime - fps_starttick >= 500)
	{
	  char buffer[256];
	  fps = (frame - fps_start_frame) / (float)((ntime - fps_starttick) / 1000.);
	  sprintf(buffer, "fps: %.1f", fps);
	  fps_start_frame = frame;
	  fps_starttick = ntime;
	  fps_fb = generate_font_buffer(font, fps_fb.buffer, buffer);
	}
      if(ntime - last_step >= 20)
	{
	  Uint32 delta = ntime - last_step;
	  last_step = ntime - (delta - 20);

	  simulate_step(&simulation, &landmass);
	}

      keystate = SDL_GetKeyboardState(NULL);
      SDL_GetMouseState(&mouse_x, &mouse_y);

      cameraPosition = new_vertex(cos(hangl) * dist * cos(vangl),
				  -sin(hangl) * dist * cos(vangl),
				  sin(vangl) * dist);
      cameraLookAt = new_vertex(0, 0, 0);
      cameraUp = new_vertex(0, 0, 1);
      sphere.r = 1.05;
      mouse_s = s_vertex_from_screen(&sphere, mouse_x, mouse_y, cameraLookAt, cameraPosition, cameraUp, fov, 0.1, &mouse_outside);

      if((selected = selected_country(&landmass, mouse_s)) > -1 && !mouse_outside)
	{
	  if(selected != last_selected)
	    {
	      if(last_selected != -1)
		{
		  update_country(&landmass, last_selected, 1, 1, 1, 0);
		}
	      update_country(&landmass, selected, 1, .5, .5, 0.01);
	      tooltip_activated = 1;
	      tooltip = generate_font_buffer(font, tooltip.buffer, landmass.countries[selected].name);
	    }
	}
      else if(last_selected != -1)
	{
	  update_country(&landmass, last_selected, 1, 1, 1, 0);
	  tooltip_activated = 0;
	}
      last_selected = selected;

      camera_speed = 1;
      if(keystate[SDL_GetScancodeFromKey(SDLK_LCTRL)])
	{
	  camera_speed = 0.2;
	}
      
      if(keystate[SDL_GetScancodeFromKey(SDLK_w)])
	{
	  vangl += 0.001 * dtime * sqrt(dist - 1.08) * camera_speed;

	  if(vangl > PI / 2 - FLT_MIN)
	    vangl = PI / 2 - FLT_MIN;
	}
      else if(keystate[SDL_GetScancodeFromKey(SDLK_s)])
	{
	  vangl -= 0.001 * dtime * sqrt(dist - 1.08) * camera_speed;

	  if(vangl < -PI / 2 + FLT_MIN)
	    vangl = -PI / 2 + FLT_MIN;
	}
      if(keystate[SDL_GetScancodeFromKey(SDLK_a)])
	hangl += 0.001 * dtime * sqrt(dist - 1.08) * camera_speed;
      else if(keystate[SDL_GetScancodeFromKey(SDLK_d)])
	hangl -= 0.001 * dtime * sqrt(dist - 1.08) * camera_speed;
      if(keystate[SDL_GetScancodeFromKey(SDLK_q)])
	{
	  dist -= (dist - 1.08) / 400 * dtime * camera_speed;
	  if(dist < 1.4)
	    dist = 1.4;
	}
      else if(keystate[SDL_GetScancodeFromKey(SDLK_e)])
	{
	  dist += (dist - 1) / 400 * dtime * camera_speed;
	  if(dist > 6)
	    dist = 6;
	}

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glMatrixMode(GL_PROJECTION);
      glLoadMatrixf(projectionv);
      glMatrixMode(GL_MODELVIEW);

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

      /* draw UI */

      glMatrixMode(GL_PROJECTION);
      glLoadMatrixf(orthov);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      glPushMatrix();
      glTranslatef(mouse_x, mouse_y, 0);

      glBegin(GL_QUADS);
      glVertex3f(-2, -2, 0);
      glVertex3f(2, -2, 0);
      glVertex3f(2, 2, 0);
      glVertex3f(-2, 2, 0);
      glEnd();
      glPopMatrix();
      glScalef(16, 16, 1);

      if(tooltip_activated)
	{
	  glPushMatrix();
	  glTranslatef(mouse_x / 16., mouse_y / 16., 0);
	  glColor3f(0, 0, 0);

	  glTranslatef(1 / 8., 1 / 8., 0);
	  draw_font_buffer(&tooltip);

	  glTranslatef(-1 / 8., -1 / 8., .1);
	  glColor3f(1, 1, 1);
	  draw_font_buffer(&tooltip);
	  glPopMatrix();
	}
      glColor3f(0, 0, 0);

      glTranslatef(1 / 8., 1 / 8., 0);
      draw_font_buffer(&fps_fb);

      glTranslatef(-1 / 8., -1 / 8., .1);
      glColor3f(1, 1, 1);
      draw_font_buffer(&fps_fb);

      SDL_GL_SwapWindow(window);

      frame++;
    }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
