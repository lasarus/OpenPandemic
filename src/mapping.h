#ifndef MAPPING_H
#define MAPPING_H

typedef struct mapper mapper_t;

typedef vertex_t (*mapper_func_t)(double, double, double);
typedef int (*mapper_intersection_func_t)(vertex_t, vertex_t, s_vertex_t *);
typedef void (*mapper_camera_func_t)(Uint8 * /*keystate*/, mapper_t /*mapper*/, Uint32 /*dtime*/, double /*camera_speed*/);
typedef void (*mapper_get_camera_func_t)(vertex_t * /*cameraPos*/, vertex_t * /*cameraup*/, vertex_t * /*cameralookAt*/, mapper_t /*mapper*/);

struct mapper
{
  mapper_func_t mapper_func;
  mapper_intersection_func_t mapper_intersection_func;

  mapper_camera_func_t mapper_camera_func;
  mapper_get_camera_func_t mapper_get_camera_func;

  void * mapper_data;
};

void map_init_sphere(mapper_t * mapper);
vertex_t map_sphere(double sector, double ring, double r);
int map_intersection_sphere(vertex_t pos, vertex_t dir, s_vertex_t * result);
void map_camera_sphere(Uint8 * keystate, mapper_t mapper, Uint32 dtime, double camera_speed);
void map_get_camera_sphere(vertex_t * cameraPos, vertex_t * cameraup, vertex_t * cameralookAt, mapper_t mapper);

void map_init_plane(mapper_t * mapper);
vertex_t map_plane(double sector, double ring, double r);
int map_intersection_plane(vertex_t pos, vertex_t dir, s_vertex_t * result);
void map_camera_plane(Uint8 * keystate, mapper_t mapper, Uint32 dtime, double camera_speed);
void map_get_camera_plane(vertex_t * cameraPos, vertex_t * cameraUp, vertex_t * cameraLookAt, mapper_t mapper);

#endif
