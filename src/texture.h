#ifndef TEXTURE_H
#define TEXTURE_H

SDL_Surface * load_surface(const char * path);
void load_texture(SDL_Surface * surface, GLuint texture);

#endif
