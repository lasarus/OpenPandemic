#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "vertex.h"
#include "land.h"

GLuint landmass_buffer;
int landmass_count;

const float landmass_radius = 1.02;

typedef struct land_data
{
  vertex_t v0;
  vertex_t c0;

  vertex_t v1;
  vertex_t c1;

  vertex_t v2;
  vertex_t c2;
} land_data_t;

static void init_landmass_buffer(landmass_t * landmass)
{
  int i, j, triangle_count = 0, c;
  land_data_t * triangles;

  for(i = 0; i < landmass->count; i++)
    {
      triangle_count += landmass->countries[i].count;
    }

  triangles = malloc(sizeof(land_data_t) * triangle_count);

  c = 0;
  for(i = 0; i < landmass->count; i++)
    {
      for(j = 0; j < landmass->countries[i].count; j++)
	{
	  s_vertex_t s_v[3];
	  s_v[0] = landmass->countries[i].triangles[j].v[0];
	  s_v[1] = landmass->countries[i].triangles[j].v[1];
	  s_v[2] = landmass->countries[i].triangles[j].v[2];
	  
	  triangles[c].v0 = spherical_coord(s_v[0], landmass_radius);
	  triangles[c].v1 = spherical_coord(s_v[1], landmass_radius);
	  triangles[c].v2 = spherical_coord(s_v[2], landmass_radius);
	  triangles[c].c0 = triangles[c].c1 = triangles[c].c2 =
	    landmass->countries[i].triangles[j].color;
	  c++;
	}
    }

  glGenBuffers(1, &landmass_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, landmass_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(land_data_t) * triangle_count,
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
  glVertexPointer(3, GL_FLOAT, sizeof(vertex_t) * 2, NULL);  
  glColorPointer(3, GL_FLOAT, sizeof(vertex_t) * 2, (void *)sizeof(vertex_t));  

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  glDrawArrays(GL_TRIANGLES, 0, landmass_count);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
