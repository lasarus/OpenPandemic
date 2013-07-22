#ifndef MAPPING_H
#define MAPPING_H

typedef vertex_t (*mapper_func_t)(double, double, double);
typedef int (*mapper_intersection_func_t)(vertex_t, vertex_t, s_vertex_t *);

typedef struct mapper
{
  mapper_func_t mapper_func;
  mapper_intersection_func_t mapper_intersection_func;
} mapper_t;

vertex_t map_sphere(double sector, double ring, double r);
int map_intersection_sphere(vertex_t pos, vertex_t dir, s_vertex_t * result);

vertex_t map_plane(double sector, double ring, double r);
int map_intersection_plane(vertex_t pos, vertex_t dir, s_vertex_t * result);

#endif
