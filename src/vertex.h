#ifndef VERTEX_H
#define VERTEX_H

#ifndef PI
#define PI 3.14159265358979
#endif

typedef struct vertex
{
  float x, y, z;
} vertex_t;

typedef struct triangle
{
  vertex_t v[3];
} triangle_t;

/* SPHERICAL VERTICES */

typedef struct s_vertex
{
  float sector, ring;
} s_vertex_t;

typedef struct s_triangle
{
  s_vertex_t v[3];
} s_triangle_t;

vertex_t spherical_coord(s_vertex_t s_vertex, float radius);

#endif
