#ifndef SPHERE_H
#define SPHERE_H

typedef struct sphere
{
  GLuint buffer;
  int sectors, rings;
  double r;
} sphere_t;

void init_sphere(sphere_t * sphere, int rings, int sectors, double r, mapper_t mapper);
void reinit_sphere(sphere_t * sphere, mapper_t mapper);
void draw_sphere(sphere_t * sphere);

s_vertex_t s_vertex_from_screen(int mouse_x, int mouse_y, vertex_t cameraLookAt, vertex_t cameraPosition, vertex_t cameraUp, float fovy, float near_clipping, int * outside, mapper_t mapper);

#endif
