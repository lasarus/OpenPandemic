#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_image.h>

#include "point.h"
#include "triangle.h"

int screen_width = 1280, screen_height = 640, screen_bpp = 32;
int quit = 0;

SDL_Window * window = NULL;
SDL_GLContext window_context;

SDL_Event event;

GLuint world_texture;

int init_opengl()
{
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);

  glClearColor(1, 0, 0, 1);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, screen_width, screen_height, 0, -1, 1);

  glMatrixMode(GL_MODELVIEW);

  if(glGetError() != GL_NO_ERROR)
    return 1;

  return 0;
}

SDL_Surface * load_surface(const char * path)
{
  SDL_Surface * surface = NULL;

  surface = IMG_Load(path);

  return surface;
}

void load_texture(SDL_Surface * surface, GLuint texture)
{
  GLenum texture_format;
  GLint nOfColors;

  nOfColors = surface->format->BytesPerPixel;
  if(nOfColors == 4)
    {
      if(surface->format->Rmask == 0x000000ff)
	texture_format = GL_RGBA;
      else
	texture_format = GL_BGRA;
    }
  else if(nOfColors == 3)
    {
      if(surface->format->Rmask == 0x000000ff)
	texture_format = GL_RGB;
      else
	texture_format = GL_BGR;
    }
  glBindTexture(GL_TEXTURE_2D, texture);
 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 
  glTexImage2D(GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
	       texture_format, GL_UNSIGNED_BYTE, surface->pixels);
}

void point_from_coord(double camera_x, double camera_y, double camera_z, int mouse_x, int mouse_y, double * mx, double * my)
{
  double tx = mouse_x, ty = mouse_y;

  tx -= screen_width / 2;
  ty -= screen_height / 2;

  tx /= camera_z;
  ty /= camera_z;

  tx += camera_x;
  ty += camera_y;

  *mx = tx;
  *my = ty;
}

int init()
{
  if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    return 1;
  
  window = SDL_CreateWindow("OpenPandemic - World Editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_OPENGL);

  if(window == NULL)
    return 0;

  window_context = SDL_GL_CreateContext(window);

  if(init_opengl())
    return 0;
  
  return 0;
}

int load_textures()
{
  SDL_Surface * tmp;

  tmp = load_surface("world.png");

  if(tmp == NULL)
    return 1;

  glGenTextures(1, &world_texture);
  load_texture(tmp, world_texture);

  SDL_FreeSurface(tmp);

  return 0;
}

const double camera_s = 10;
const double camera_z_s = 1.1;
int main(int argc, char ** argv)
{
  double camera_x = 0, camera_y = 0, camera_z = 1;
  point_list_t point_list;
  triangle_list_t triangle_list;
  int mouse_x, mouse_y;

  int mode = 0;
  int delete = 0;

  point_t p1, p2;
  int stage = 0;

  if(init())
    return 1;

  if(load_textures())
    return 1;

  init_point_list(&point_list);
  init_triangle_list(&triangle_list);

  while(!quit)
    {
      const Uint8 * keystate = NULL;
      while(SDL_PollEvent(&event))
	{
	  if(event.type == SDL_QUIT)
	    {
	      quit = 1;
	    }
	  else if(event.type == SDL_MOUSEBUTTONDOWN)
	    {
	      double mx, my;

	      mouse_x = event.button.x;
	      mouse_y = event.button.y;

	      point_from_coord(camera_x, camera_y, camera_z, mouse_x, mouse_y, &mx, &my);

	      if(mode)
		{
		  if(stage == 0)
		    {
		      p1 = get_closest_point(&point_list, mx, my);
		      stage++;
		    }
		  else if(stage == 1)
		    {
		      p2 = get_closest_point(&point_list, mx, my);
		      stage++;
		    }
		  else if(stage == 2)
		    {
		      point_t p3;
		      p3 = get_closest_point(&point_list, mx, my);
		      stage = 0;

		      add_triangle(&triangle_list, new_triangle(p1, p2, p3));
		    }
		}
	      else
		{
		  if(delete)
		    {
		      int closest_i;
		      closest_i = get_closest_point_i(&point_list, mx, my);
		      remove_point(&point_list, closest_i);
		    }
		  else
		    {
		      add_point(&point_list, new_point(mx, my));
		    }
		}
	    }
	  else if(event.type == SDL_KEYDOWN)
	    {
	      SDL_Keycode key = event.key.keysym.sym;

	      if(key == SDLK_SPACE)
		{
		  mode = !mode;
		  printf("Mode switched: %i\n", mode);
		}
	      else if(key == SDLK_DELETE)
		{
		  delete = !delete;
		  printf("Delete switched: %i\n", delete);
		}
	      else if(key == SDLK_p)
		{
		  print_triangles(&triangle_list);
		}
	      else if(key == SDLK_c)
		{
		  free(triangle_list.triangles);
		  triangle_list.triangles = NULL;
		  triangle_list.count = 0;
		}
	      else if(key == SDLK_l)
		{
		  free(point_list.points);
		  load_points(&point_list, "points.opwbp");
		  printf("Loaded points!\n");
		}
	      else if(key == SDLK_o)
		{
		  save_points(&point_list, "points.opwbp");
		  printf("Saved points!\n");
		}
	    }
	}

      SDL_GetMouseState(&mouse_x, &mouse_y);
      keystate = SDL_GetKeyboardState(NULL);

      if(keystate[SDL_GetScancodeFromKey(SDLK_w)])
	{
	  camera_y -= camera_s / camera_z;
	}
      else if(keystate[SDL_GetScancodeFromKey(SDLK_s)])
	{
	  camera_y += camera_s / camera_z;
	}
      if(keystate[SDL_GetScancodeFromKey(SDLK_a)])
	{
	  camera_x -= camera_s / camera_z;
	}
      else if(keystate[SDL_GetScancodeFromKey(SDLK_d)])
	{
	  camera_x += camera_s / camera_z;
	}
      if(keystate[SDL_GetScancodeFromKey(SDLK_q)])
	{
	  camera_z *= camera_z_s;

	  if(camera_z > 40)
	    camera_z = 40;
	}
      else if(keystate[SDL_GetScancodeFromKey(SDLK_e)])
	{
	  camera_z /= camera_z_s;
	  if(camera_z < 1)
	    camera_z = 1;
	}

      if(camera_x < screen_width / 2 / camera_z)
	camera_x = screen_width / 2 / camera_z;
      if(camera_x > screen_width - (screen_width / (2 * camera_z)))
	camera_x = screen_width - (screen_width / (2 * camera_z));

      if(camera_y < screen_height / 2 / camera_z)
	camera_y = screen_height / 2 / camera_z;
      if(camera_y > screen_height / 2 / camera_z + screen_height - screen_height / camera_z)
	camera_y = screen_height / 2 / camera_z + screen_height - screen_height / camera_z;
 
      glClear(GL_COLOR_BUFFER_BIT);
      glLoadIdentity();

      glTranslatef(screen_width / 2, screen_height / 2, 0);
      glScalef(camera_z, camera_z, 1);
      glTranslatef(-camera_x, -camera_y, 0);

      glBindTexture(GL_TEXTURE_2D, world_texture);

      glBegin(GL_QUADS);
      
      glColor3f(1, 1, 1);

      glTexCoord2i(0, 0);
      glVertex3f(0, 0, 0);      
      glTexCoord2i(1, 0);
      glVertex3f(screen_width, 0, 0);
      glTexCoord2i(1, 1);
      glVertex3f(screen_width, screen_height, 0);
      glTexCoord2i(0, 1);
      glVertex3f(0, screen_height, 0);
      glEnd();

      glBindTexture(GL_TEXTURE_2D, 0);

      {
	double mx, my;
	
	point_from_coord(camera_x, camera_y, camera_z, mouse_x, mouse_y, &mx, &my);
	draw_point_list(&point_list, mx, my);
	draw_triangle_list(&triangle_list, mx, my);
      }

      SDL_GL_SwapWindow(window);
    }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
