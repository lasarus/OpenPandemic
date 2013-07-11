#ifndef LAND_H
#define LAND_H

typedef struct country
{
  int count;
  s_triangle_t * triangles;
} country_t;

typedef struct landmass
{
  int count;
  country_t * countries;
} landmass_t;

void init_landmass(landmass_t * landmass);
void draw_landmass(landmass_t * landmass);

#endif
