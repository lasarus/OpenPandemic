#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <math.h>
#include <float.h>

#include "vertex.h"
#include "mapping.h"

typedef struct mapper_sphere_data
{
  double vangl, hangl, dist;
} mapper_sphere_data_t;

void map_init_sphere(mapper_t * mapper)
{
  mapper_sphere_data_t * mapper_sphere_data;

  mapper_sphere_data = malloc(sizeof(mapper_sphere_data_t));

  mapper_sphere_data->vangl = 0;
  mapper_sphere_data->hangl = 0;
  mapper_sphere_data->dist = 4;

  mapper->mapper_data = mapper_sphere_data;
}

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

void map_camera_sphere(Uint8 * keystate, mapper_t mapper, Uint32 dtime, double camera_speed)
{
  mapper_sphere_data_t * mapper_sphere_data;
  mapper_sphere_data = mapper.mapper_data;

  if(keystate[SDL_GetScancodeFromKey(SDLK_w)])
    {
      mapper_sphere_data->vangl += 0.001 * dtime * sqrt(mapper_sphere_data->dist - 1.08) * camera_speed;

      if(mapper_sphere_data->vangl > PI / 2 - FLT_MIN)
	mapper_sphere_data->vangl = PI / 2 - FLT_MIN;
    }
  else if(keystate[SDL_GetScancodeFromKey(SDLK_s)])
    {
      mapper_sphere_data->vangl -= 0.001 * dtime * sqrt(mapper_sphere_data->dist - 1.08) * camera_speed;

      if(mapper_sphere_data->vangl < -PI / 2 + FLT_MIN)
	mapper_sphere_data->vangl = -PI / 2 + FLT_MIN;
    }
  if(keystate[SDL_GetScancodeFromKey(SDLK_a)])
    mapper_sphere_data->hangl += 0.001 * dtime * sqrt(mapper_sphere_data->dist - 1.08) * camera_speed;
  else if(keystate[SDL_GetScancodeFromKey(SDLK_d)])
    mapper_sphere_data->hangl -= 0.001 * dtime * sqrt(mapper_sphere_data->dist - 1.08) * camera_speed;
  if(keystate[SDL_GetScancodeFromKey(SDLK_q)])
    {
      mapper_sphere_data->dist -= (mapper_sphere_data->dist - 1.08) / 400 * dtime * camera_speed;
      if(mapper_sphere_data->dist < 1.4)
	mapper_sphere_data->dist = 1.4;
    }
  else if(keystate[SDL_GetScancodeFromKey(SDLK_e)])
    {
      mapper_sphere_data->dist += (mapper_sphere_data->dist - 1) / 400 * dtime * camera_speed;
      if(mapper_sphere_data->dist > 6)
	mapper_sphere_data->dist = 6;
    }
}

void map_get_camera_sphere(vertex_t * cameraPos, vertex_t * cameraUp, vertex_t * cameraLookAt, mapper_t mapper)
{
  mapper_sphere_data_t * mapper_sphere_data;
  mapper_sphere_data = mapper.mapper_data;

  *cameraPos = new_vertex(cos(mapper_sphere_data->hangl) * mapper_sphere_data->dist *
			       cos(mapper_sphere_data->vangl),

			       -sin(mapper_sphere_data->hangl) * mapper_sphere_data->dist *
			       cos(mapper_sphere_data->vangl),

			       sin(mapper_sphere_data->vangl) * mapper_sphere_data->dist);
  *cameraLookAt = new_vertex(0, 0, 0);
  *cameraUp = new_vertex(0, 0, 1);
}

/*----------- PLANE ------------*/

typedef struct mapper_plane_data
{
  double x, y;
  double dist;
} mapper_plane_data_t;

void map_init_plane(mapper_t * mapper)
{
  mapper_plane_data_t * mapper_plane_data;
  mapper_plane_data = malloc(sizeof(mapper_plane_data_t));

  mapper_plane_data->x = 0;
  mapper_plane_data->y = 0;
  mapper_plane_data->dist = 1;

  mapper->mapper_data = mapper_plane_data;
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

void map_camera_plane(Uint8 * keystate, mapper_t mapper, Uint32 dtime, double camera_speed)
{
  mapper_plane_data_t * mapper_plane_data;
  mapper_plane_data = mapper.mapper_data;

  double dist = mapper_plane_data->dist - 0.1;

  if(keystate[SDL_GetScancodeFromKey(SDLK_w)])
    {
      mapper_plane_data->y += sqrt(dist) * dtime * 0.001 * camera_speed;
    }
  else if(keystate[SDL_GetScancodeFromKey(SDLK_s)])
    {
      mapper_plane_data->y -= sqrt(dist) * dtime * 0.001 * camera_speed;
    }
  if(keystate[SDL_GetScancodeFromKey(SDLK_a)])
    {
      mapper_plane_data->x -= sqrt(dist) * dtime * 0.001 * camera_speed;
    }
  else if(keystate[SDL_GetScancodeFromKey(SDLK_d)])
    {
      mapper_plane_data->x += sqrt(dist) * dtime * 0.001 * camera_speed;
    }

  if(mapper_plane_data->x < -1)
    mapper_plane_data->x = -1;
  else if(mapper_plane_data->x > 1)
    mapper_plane_data->x = 1;

  if(mapper_plane_data->y < -.5)
    mapper_plane_data->y = -.5;
  else if(mapper_plane_data->y > .5)
    mapper_plane_data->y = .5;

  if(keystate[SDL_GetScancodeFromKey(SDLK_q)])
    {
      mapper_plane_data->dist -= dist * dtime * 0.002 * camera_speed;
      if(mapper_plane_data->dist < 0.2)
	mapper_plane_data->dist = 0.2;
    }
  else if(keystate[SDL_GetScancodeFromKey(SDLK_e)])
    {
      mapper_plane_data->dist += dist * dtime * 0.003 * camera_speed;
      if(mapper_plane_data->dist > 3)
	mapper_plane_data->dist = 3;
    }
}

void map_get_camera_plane(vertex_t * cameraPos, vertex_t * cameraUp, vertex_t * cameraLookAt, mapper_t mapper)
{
  mapper_plane_data_t * mapper_plane_data;
  mapper_plane_data = mapper.mapper_data;

  double x, y, dist;
  x = mapper_plane_data->x;
  y = mapper_plane_data->y;
  dist = mapper_plane_data->dist;

  *cameraPos = new_vertex(x, y - 0.1, dist);
  *cameraLookAt = new_vertex(x, y, 0);
  *cameraUp = new_vertex(0, 0, 1);
}
