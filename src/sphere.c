#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "vertex.h"
#include "sphere.h"

void init_sphere(sphere_t * sphere, int rings, int sectors, double r)
{
  vertex_t * sphere_data;
  int i, j;

  sphere_data = malloc(sizeof(vertex_t) * rings
		  * sectors * 4);

  sphere->rings = rings;
  sphere->sectors = sectors;
  sphere->r = r;

  for(i = 0; i < sectors; i++)
    {
      double sector_angl, prev_sector_angl;

      sector_angl = (i / (double)sectors) * PI * 2;
      prev_sector_angl = ((i - 1) / (double)sectors) * PI * 2;
      for(j = 1; j < rings + 1; j++)
	{
	  double ring_angl, prev_ring_angl;
	  int base_index = ((j - 1) + i * rings) * 4;

	  ring_angl = (j / (double)rings - 0.5) * PI;
	  prev_ring_angl = ((j - 1) / (double)rings - 0.5) * PI;

	  sphere_data[base_index + 0].x = cos(prev_sector_angl) * cos(prev_ring_angl);
	  sphere_data[base_index + 0].y = -sin(prev_sector_angl) * cos(prev_ring_angl);
	  sphere_data[base_index + 0].z = sin(prev_ring_angl);

	  sphere_data[base_index + 1].x = cos(prev_sector_angl) * cos(ring_angl);
	  sphere_data[base_index + 1].y = -sin(prev_sector_angl) * cos(ring_angl);
	  sphere_data[base_index + 1].z = sin(ring_angl);

	  sphere_data[base_index + 2].x = cos(sector_angl) * cos(ring_angl);
	  sphere_data[base_index + 2].y = -sin(sector_angl) * cos(ring_angl);
	  sphere_data[base_index + 2].z = sin(ring_angl);

	  sphere_data[base_index + 3].x = cos(sector_angl) * cos(prev_ring_angl);
	  sphere_data[base_index + 3].y = -sin(sector_angl) * cos(prev_ring_angl);
	  sphere_data[base_index + 3].z = sin(prev_ring_angl);
	}
    }

  glGenBuffers(1, &sphere->buffer);

  glBindBuffer(GL_ARRAY_BUFFER, sphere->buffer);
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(vertex_t) * rings * sectors * 4,
	       sphere_data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  free(sphere_data);
}

void draw_sphere(sphere_t * sphere)
{
  glBindBuffer(GL_ARRAY_BUFFER, sphere->buffer);
  glVertexPointer(3, GL_FLOAT, 0, NULL);  
  glEnableClientState(GL_VERTEX_ARRAY);

  glDrawArrays(GL_QUADS, 0, sphere->rings * sphere->sectors * 4);

  glDisableClientState(GL_VERTEX_ARRAY);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
