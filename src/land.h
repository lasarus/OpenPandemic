#ifndef LAND_H
#define LAND_H

typedef struct c_triangle
{
  vertex_t color;
  s_vertex_t v[3];
} c_triangle_t;

typedef struct country
{
  char * name;
  int population;
  
  int vbo_start;

  int count;
  c_triangle_t * triangles;
} country_t;

typedef struct landmass
{
  int count;
  country_t * countries;
} landmass_t;

void init_landmass(landmass_t * landmass);
void draw_landmass(landmass_t * landmass);

void update_country(landmass_t * landmass, int i, float r, float g, float b, double h);

int selected_country(landmass_t * landmass, s_vertex_t mouse_s);

#endif
