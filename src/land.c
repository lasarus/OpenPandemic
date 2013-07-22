#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "vertex.h"
#include "mapping.h"
#include "land.h"

GLuint landmass_buffer;
int landmass_count;

const float landmass_radius = 1.02;

typedef struct land_data
{
  vertex_t v0;
  vertex_t c0;

  vertex_t v1;
  vertex_t c1;

  vertex_t v2;
  vertex_t c2;
} land_data_t;

static void update_country_buffer(country_t country, land_data_t * triangles, int * c,
				  float r, float g, float b, double h, mapper_t mapper)
{
  int i;
  mapper_func_t mapper_func = mapper.mapper_func;

  for(i = 0; i < country.count; i++)
    {
      s_vertex_t s_v[3];
      s_v[0] = country.triangles[i].v[0];
      s_v[1] = country.triangles[i].v[1];
      s_v[2] = country.triangles[i].v[2];

      triangles[*c].v0 = (*mapper_func)(s_v[0].sector, s_v[0].ring, landmass_radius + h);
      triangles[*c].v1 = (*mapper_func)(s_v[1].sector, s_v[1].ring, landmass_radius + h);
      triangles[*c].v2 = (*mapper_func)(s_v[2].sector, s_v[2].ring, landmass_radius + h);

      triangles[*c].c0.x = triangles[*c].c1.x = triangles[*c].c2.x = country.triangles[i].color.x * r;
      triangles[*c].c0.y = triangles[*c].c1.y = triangles[*c].c2.y = country.triangles[i].color.y * g;
      triangles[*c].c0.z = triangles[*c].c1.z = triangles[*c].c2.z = country.triangles[i].color.z * b;
      
      (*c)++;
    }
}

static void init_landmass_buffer(landmass_t * landmass, mapper_t mapper)
{
  int i, j, triangle_count = 0, c;
  land_data_t * triangles;

  for(i = 0; i < landmass->count; i++)
    {
      triangle_count += landmass->countries[i].count;
    }

  triangles = malloc(sizeof(land_data_t) * triangle_count);

  c = 0;
  for(i = 0; i < landmass->count; i++)
    {
      landmass->countries[i].vbo_start = c * sizeof(land_data_t);
      update_country_buffer(landmass->countries[i], triangles, &c, 1, 1, 1, 0, mapper);
    }

  glGenBuffers(1, &landmass_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, landmass_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(land_data_t) * triangle_count,
	       triangles, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  landmass_count = triangle_count * 3;
}

void update_country(landmass_t * landmass, int i,
		    float r, float g, float b, double h, mapper_t mapper)
{
  land_data_t * triangles;
  country_t * country;
  int j, c;

  country = &landmass->countries[i];

  triangles = malloc(sizeof(land_data_t) * country->count);

  c = 0;
  update_country_buffer(*country, triangles, &c, r, g, b, h, mapper);
  
  glBindBuffer(GL_ARRAY_BUFFER, landmass_buffer);
  glBufferSubData(GL_ARRAY_BUFFER, country->vbo_start, sizeof(land_data_t) * country->count, triangles);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  free(triangles);
}

void init_landmass(landmass_t * landmass, mapper_t mapper)
{
  init_landmass_buffer(landmass, mapper);
}

void draw_landmass(landmass_t * landmass)
{
  glBindBuffer(GL_ARRAY_BUFFER, landmass_buffer);
  glVertexPointer(3, GL_FLOAT, sizeof(vertex_t) * 2, NULL);  
  glColorPointer(3, GL_FLOAT, sizeof(vertex_t) * 2, (void *)sizeof(vertex_t));  

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  glDrawArrays(GL_TRIANGLES, 0, landmass_count);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static float sign(s_vertex_t p1, s_vertex_t p2, s_vertex_t p3)
{
  return (p1.sector - p3.sector) * (p2.ring - p3.ring) - (p2.sector - p3.sector) * (p1.ring - p3.ring);
}

static int point_in_triangle(s_vertex_t pt, s_vertex_t v1, s_vertex_t v2, s_vertex_t v3)
{
  int s1, s2, s3;
  s1 = sign(pt, v1, v2) < 0.0f;
  s2 = sign(pt, v2, v3) < 0.0f;
  s3 = sign(pt, v3, v1) < 0.0f;
  return (s1 == s2) && (s2 == s3);
}

int selected_country(landmass_t * landmass, s_vertex_t mouse_s)
{
  int i, j;

  for(i = 0; i < landmass->count; i++)
    {
      if(!landmass->countries[i].selectable)
	continue;
      for(j = 0; j < landmass->countries[i].count; j++)
	{
	  c_triangle_t triangle = landmass->countries[i].triangles[j];
	  if(point_in_triangle(mouse_s, triangle.v[0], triangle.v[1], triangle.v[2]))
	    return i;
	}
    }
  return -1;
}
