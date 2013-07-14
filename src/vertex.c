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

vertex_t new_vertex(double x, double y, double z)
{
  vertex_t v;

  v.x = x;
  v.y = y;
  v.z = z;

  return v;
}

vertex_t normalize_vector(vertex_t v)
{
  double len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

  v.x /= len;
  v.y /= len;
  v.z /= len;

  return v;
}

vertex_t cross_product(vertex_t a, vertex_t b)
{
  vertex_t v;

  v.x = a.y * b.z - a.z * b.y;
  v.y = a.z * b.x - a.x * b.z;
  v.z = a.x * b.y - a.y * b.x;

  return v;
}

double dot_product(vertex_t a, vertex_t b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

vertex_t subtract_vector(vertex_t a, vertex_t b)
{
  vertex_t v;

  v.x = a.x - b.x;
  v.y = a.y - b.y;
  v.z = a.z - b.z;

  return v;
}

vertex_t add_vector(vertex_t a, vertex_t b)
{
  vertex_t v;

  v.x = a.x + b.x;
  v.y = a.y + b.y;
  v.z = a.z + b.z;

  return v;
}

vertex_t scale_vector(vertex_t v, double scalar)
{
  v.x *= scalar;
  v.y *= scalar;
  v.z *= scalar;

  return v;
}

double vector_lenght(vertex_t v)
{
  return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
