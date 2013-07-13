#ifndef TRIANGLE_H
#define TRIANGLE_H

typedef struct triangle
{
  point_t p[3];
} triangle_t;

typedef struct triangle_list
{
  int count;
  triangle_t * triangles;
} triangle_list_t;

triangle_t new_triangle(point_t p1, point_t p2, point_t p3);

void init_triangle_list(triangle_list_t * triangle_list);
void add_triangle(triangle_list_t * triangle_list, triangle_t triangle);

void draw_triangle_list(triangle_list_t * triangle_list, double mx, double my);

void print_triangles(triangle_list_t * triangle_list);

#endif
