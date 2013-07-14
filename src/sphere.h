#ifndef SPHERE_H
#define SPHERE_H

typedef struct sphere
{
  GLuint buffer;
  int sectors, rings;
  double r;
} sphere_t;

void init_sphere(sphere_t * sphere, int rings, int sectors, double r);
void draw_sphere(sphere_t * sphere);

#endif
