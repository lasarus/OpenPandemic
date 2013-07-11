#include <math.h>
#include "vertex.h"

vertex_t spherical_coord(s_vertex_t s_vertex, float radius)
{
  vertex_t vertex;

  vertex.z = sin(s_vertex.ring * (PI / 2)) * radius;
  vertex.x = cos(s_vertex.sector * PI) *
    cos(s_vertex.ring * (PI / 2)) * radius;
  vertex.y = -sin(s_vertex.sector * PI) *
    cos(s_vertex.ring * (PI / 2)) * radius;

  return vertex;
}
