#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "vertex.h"
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

static void init_landmass_buffer(landmass_t * landmass)
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
      for(j = 0; j < landmass->countries[i].count; j++)
	{
	  s_vertex_t s_v[3];
	  s_v[0] = landmass->countries[i].triangles[j].v[0];
	  s_v[1] = landmass->countries[i].triangles[j].v[1];
	  s_v[2] = landmass->countries[i].triangles[j].v[2];
	  
	  triangles[c].v0 = spherical_coord(s_v[0], landmass_radius);
	  triangles[c].v1 = spherical_coord(s_v[1], landmass_radius);
	  triangles[c].v2 = spherical_coord(s_v[2], landmass_radius);
	  triangles[c].c0 = triangles[c].c1 = triangles[c].c2 =
	    landmass->countries[i].triangles[j].color;
	  c++;
	}
    }

  glGenBuffers(1, &landmass_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, landmass_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(land_data_t) * triangle_count,
	       triangles, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  landmass_count = triangle_count * 3;
}

void update_country(landmass_t * landmass, int i, float r, float g, float b, double h)
{
  land_data_t * triangles;
  country_t * country;
  int j;

  country = &landmass->countries[i];

  triangles = malloc(sizeof(land_data_t) * country->count);
  
  for(j = 0; j < landmass->countries[i].count; j++)
    {
      s_vertex_t s_v[3];
      s_v[0] = landmass->countries[i].triangles[j].v[0];
      s_v[1] = landmass->countries[i].triangles[j].v[1];
      s_v[2] = landmass->countries[i].triangles[j].v[2];
	  
      triangles[j].v0 = spherical_coord(s_v[0], landmass_radius + h);
      triangles[j].v1 = spherical_coord(s_v[1], landmass_radius + h);
      triangles[j].v2 = spherical_coord(s_v[2], landmass_radius + h);
      triangles[j].c0.x = triangles[j].c1.x = triangles[j].c2.x = landmass->countries[i].triangles[j].color.x * r;
      triangles[j].c0.y = triangles[j].c1.y = triangles[j].c2.y = landmass->countries[i].triangles[j].color.y * g;
      triangles[j].c0.z = triangles[j].c1.z = triangles[j].c2.z = landmass->countries[i].triangles[j].color.z * b;
    }

  glBindBuffer(GL_ARRAY_BUFFER, landmass_buffer);
  glBufferSubData(GL_ARRAY_BUFFER, country->vbo_start, sizeof(land_data_t) * country->count, triangles);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  free(triangles);
}

void init_landmass(landmass_t * landmass)
{
  init_landmass_buffer(landmass);
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
      for(j = 0; j < landmass->countries[i].count; j++)
	{
	  c_triangle_t triangle = landmass->countries[i].triangles[j];
	  if(point_in_triangle(mouse_s, triangle.v[0], triangle.v[1], triangle.v[2]))
	    return i;
	}
    }
  return -1;
}
