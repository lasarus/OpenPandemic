#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "vertex.h"
#include "sphere.h"

extern int screen_height, screen_width;

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

s_vertex_t s_vertex_from_screen(sphere_t * sphere, int mouse_x, int mouse_y, vertex_t cameraLookAt, vertex_t cameraPosition, vertex_t cameraUp, float fovy, float near_clipping, int * outside)
{
  s_vertex_t result;
  vertex_t dir, pos;
  vertex_t relative_vector;

  pick_ray(mouse_x, mouse_y, cameraLookAt, cameraPosition, cameraUp, fovy, near_clipping, &pos, &dir);

  /* SPHERE LINE INTERSECTION */
  if(line_sphere_intersection(sphere, pos, dir, &relative_vector))
    {
      *outside = 1;
      return result; /* not usable */
    }
  *outside = 0;

  /* SPHERE COORS FROM RELATIVE VECTOR */

  result.sector = (atan2(-relative_vector.y, relative_vector.x) / PI);
  result.ring = -(atan2(-relative_vector.z,
		       sqrt(relative_vector.y * relative_vector.y +
			    relative_vector.x * relative_vector.x)) / (PI / 2));

  return result;
}
