#ifndef VERTEX_H
#define VERTEX_H

#ifndef PI
#define PI 3.14159265358979
#endif

typedef struct vertex
{
  float x, y, z;
} vertex_t;

typedef struct color
{
  float r, g, b;
} color_t;

typedef struct tex_coord
{
  float x, y;
} tex_coord_t;

typedef struct vertex vector_t;

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

vertex_t new_vertex(double x, double y, double z);
vertex_t normalize_vector(vertex_t v);
vertex_t cross_product(vertex_t a, vertex_t b);
double dot_product(vertex_t a, vertex_t b);
vertex_t add_vector(vertex_t a, vertex_t b);
vertex_t subtract_vector(vertex_t a, vertex_t b);
vertex_t scale_vector(vertex_t v, double scalar);
double vector_lenght(vertex_t v);

#endif
