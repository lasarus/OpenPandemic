#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "vertex.h"
#include "land.h"

GLuint landmass_buffer;
int landmass_count;

const float landmass_radius = 1.02;

static void init_landmass_buffer(landmass_t * landmass)
{
  int i, j, triangle_count = 0, c;
  triangle_t * triangles;

  for(i = 0; i < landmass->count; i++)
    {
      triangle_count += landmass->countries[i].count;
    }

  triangles = malloc(sizeof(triangle_t) * triangle_count);

  c = 0;
  for(i = 0; i < landmass->count; i++)
    {
      for(j = 0; j < landmass->countries[i].count; j++)
	{
	  s_vertex_t s_v[3];
	  s_v[0] = landmass->countries[i].triangles[j].v[0];
	  s_v[1] = landmass->countries[i].triangles[j].v[1];
	  s_v[2] = landmass->countries[i].triangles[j].v[2];
	  
	  triangles[c].v[0] = spherical_coord(s_v[0], landmass_radius);
	  triangles[c].v[1] = spherical_coord(s_v[1], landmass_radius);
	  triangles[c].v[2] = spherical_coord(s_v[2], landmass_radius);
	  c++;
	}
    }

  glGenBuffers(1, &landmass_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, landmass_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_t) * triangle_count,
	       triangles, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  landmass_count = triangle_count * 3;
}

void init_landmass(landmass_t * landmass)
{
  init_landmass_buffer(landmass);
}

void draw_landmass(landmass_t * landmass)
{
  glBindBuffer(GL_ARRAY_BUFFER, landmass_buffer);
  glVertexPointer(3, GL_FLOAT, 0, NULL);  
  glEnableClientState(GL_VERTEX_ARRAY);

  glDrawArrays(GL_TRIANGLES, 0, landmass_count);

  glDisableClientState(GL_VERTEX_ARRAY);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
