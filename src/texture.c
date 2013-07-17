#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>

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
