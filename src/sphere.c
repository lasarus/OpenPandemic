#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "vertex.h"
#include "mapping.h"
#include "sphere.h"
#include "mapping.h"

extern int screen_height, screen_width;

void reinit_sphere(sphere_t * sphere, mapper_t mapper)
{
  vertex_t * sphere_data;
  mapper_func_t mapper_func = mapper.mapper_func;
  int i, j;

  sphere_data = malloc(sizeof(vertex_t) * sphere->rings
		  * sphere->sectors * 4);
  
  for(i = 0; i < sphere->sectors; i++)
    {
      double sector, prev_sector;

      sector = ((i + 1) / (double)sphere->sectors) * 2 - 1;
      prev_sector = (i / (double)sphere->sectors) * 2 - 1;
      for(j = 1; j < sphere->rings + 1; j++)
	{
	  double ring, prev_ring;
	  int base_index = ((j - 1) + i * sphere->rings) * 4;

	  ring = (j / (double)sphere->rings - 0.5) * 2;
	  prev_ring = ((j - 1) / (double)sphere->rings - 0.5) * 2;

	  sphere_data[base_index + 0] = (*mapper_func)(prev_sector, prev_ring, 1);
	  sphere_data[base_index + 1] = (*mapper_func)(prev_sector, ring, 1);
	  sphere_data[base_index + 2] = (*mapper_func)(sector, ring, 1);
	  sphere_data[base_index + 3] = (*mapper_func)(sector, prev_ring, 1);
	}
    }

  glBindBuffer(GL_ARRAY_BUFFER, sphere->buffer);
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(vertex_t) * sphere->rings * sphere->sectors * 4,
	       sphere_data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  free(sphere_data);
}

void init_sphere(sphere_t * sphere, int rings, int sectors, double r, mapper_t mapper)
{
  sphere->rings = rings;
  sphere->sectors = sectors;
  sphere->r = r;

  glGenBuffers(1, &sphere->buffer);

  reinit_sphere(sphere, mapper);
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

void pick_ray(int xi, int yi, vertex_t cameraLookAt, vertex_t cameraPos, vertex_t cameraUp, float fovy, float near_clipping, vertex_t * pos, vertex_t * dir)
{
  vertex_t view, h, v;
  double x, y;
  float rad;
  float vlen, hlen;

  x = xi;
  y = yi;

  view = subtract_vector(cameraLookAt, cameraPos);
  view = normalize_vector(view);

  h = cross_product(view, cameraUp);
  h = normalize_vector(h);

  v = cross_product(view, h);
  v = normalize_vector(v);

  rad = fovy / 180. * PI;
  vlen = tan(rad / 2) * near_clipping;
  hlen = vlen * ((float)screen_width / (float)screen_height);

  v = scale_vector(v, vlen);
  h = scale_vector(h, hlen);

  x -= (float)screen_width / 2;
  y -= (float)screen_height / 2;

  y /= ((float)screen_height / 2);
  x /= ((float)screen_width / 2);

  pos->x = cameraPos.x + view.x * near_clipping + h.x * x + v.x * y;
  pos->y = cameraPos.y + view.y * near_clipping + h.y * x + v.y * y;
  pos->z = cameraPos.z + view.z * near_clipping + h.z * x + v.z * y;

  *dir = subtract_vector(*pos, cameraPos);
  *dir = normalize_vector(*dir);
}

int line_sphere_intersection(sphere_t * sphere, vertex_t pos, vertex_t dir, vertex_t * result)
{
  double d, under_sqrt;
  double a;
  vertex_t b;

  a = dot_product(dir, subtract_vector(pos, new_vertex(0, 0, 0)));
  b = subtract_vector(pos, new_vertex(0, 0, 0));

  under_sqrt = pow(a, 2) - dot_product(b, b) + sphere->r;

  if(under_sqrt < 0)
    {
      return 1;
    }

  d = -(dot_product(dir, subtract_vector(pos, new_vertex(0, 0, 0)))) -
    sqrt(under_sqrt);

  *result = add_vector(pos, scale_vector(dir, d));

  return 0;
}

s_vertex_t s_vertex_from_screen(int mouse_x, int mouse_y, vertex_t cameraLookAt, vertex_t cameraPosition, vertex_t cameraUp, float fovy, float near_clipping, int * outside, mapper_t mapper)
{
  s_vertex_t result;
  vertex_t dir, pos;
  mapper_intersection_func_t mapper_intersection_func = mapper.mapper_intersection_func;

  pick_ray(mouse_x, mouse_y, cameraLookAt, cameraPosition, cameraUp, fovy, near_clipping, &pos, &dir);

  /* SPHERE LINE INTERSECTION */
  if((*mapper_intersection_func)(pos, dir, &result))
    {
      *outside = 1;
      return result; /* not usable */
    }

  *outside = 0;
  return result;
}
