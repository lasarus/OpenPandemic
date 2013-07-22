#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <math.h>

#include "vertex.h"

vertex_t map_sphere(double sector, double ring, double r)
{
  vertex_t vertex;

  vertex.z = sin(ring * (PI / 2)) * r;
  vertex.x = cos(sector * PI) * cos(ring * (PI / 2)) * r;
  vertex.y = -sin(sector * PI) * cos(ring * (PI / 2)) * r;

  return vertex;
}

int map_intersection_sphere(vertex_t pos, vertex_t dir, s_vertex_t * result)
{
  double d, under_sqrt;
  double radius;
  double a;
  vertex_t b, relative_vector;
  radius = 1.05;

  a = dot_product(dir, subtract_vector(pos, new_vertex(0, 0, 0)));
  b = subtract_vector(pos, new_vertex(0, 0, 0));

  under_sqrt = pow(a, 2) - dot_product(b, b) + radius;

  if(under_sqrt < 0)
    {
      return 1;
    }

  d = -(dot_product(dir, subtract_vector(pos, new_vertex(0, 0, 0)))) -
    sqrt(under_sqrt);

  relative_vector = add_vector(pos, scale_vector(dir, d));

  result->sector = (atan2(-relative_vector.y, relative_vector.x) / PI);
  result->ring = -(atan2(-relative_vector.z,
			 sqrt(relative_vector.y * relative_vector.y +
			      relative_vector.x * relative_vector.x)) / (PI / 2));

  return 0;
}

vertex_t map_plane(double sector, double ring, double r)
{
  vertex_t vertex;

  vertex.z = r - 1;

  vertex.x = -sector;
  vertex.y = ring / 2;

  return vertex;
}

int map_intersection_plane(vertex_t pos, vertex_t dir, s_vertex_t * result)
{
  double z;

  /* pos.z + dir.z * z = 0.02 */
  /* dir.z * z = 0.02 - pos.z */
  /* z = (0.02 - pos.z) / dir.z */

  z = (0.02 - pos.z) / dir.z;
  
  result->sector = -(pos.x + dir.x * z);
  result->ring = (pos.y + dir.y * z) * 2;

  return 0;
}
