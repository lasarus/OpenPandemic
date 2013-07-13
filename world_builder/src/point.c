#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <float.h>

#include "point.h"

point_t new_point(double x, double y)
{
  point_t p;

  p.x = x;
  p.y = y;

  return p;
}

void init_point_list(point_list_t * point_list)
{
  point_list->count = 0;
  point_list->points = NULL;
}

void add_point(point_list_t * point_list, point_t point)
{
  point_list->count++;
  point_list->points = realloc(point_list->points, sizeof(point_t) *
			       point_list->count);

  point_list->points[point_list->count - 1] = point;
}

void remove_point(point_list_t * point_list, int i)
{
  if(point_list->count == 0)
    return;

  if(i != point_list->count - 1)
    memmove(&point_list->points[i], &point_list->points[i + 1], sizeof(point_t) * (point_list->count - i - 1));

  point_list->count--;
  point_list->points = realloc(point_list->points, sizeof(point_t) *
			       point_list->count);
}

int get_closest_point_i(point_list_t * point_list, double x, double y)
{
  int i;
  double closest_dist = DBL_MAX;
  int closest_point;

  for(i = 0; i < point_list->count; i++)
    {
      double dist;

      dist = pow(point_list->points[i].x - x, 2)
	+ pow(point_list->points[i].y - y, 2);

      if(dist < closest_dist)
	{
	  closest_dist = dist;
	  closest_point = i;
	}
    }
  return closest_point;
}

point_t get_closest_point(point_list_t * point_list, double x, double y)
{
  int i = get_closest_point_i(point_list, x, y);
  return point_list->points[i];
}

void draw_point_list(point_list_t * point_list, double mx, double my)
{
  int i;
  int closest_i = get_closest_point_i(point_list, mx, my);

  glBegin(GL_QUADS);
  for(i = 0; i < point_list->count; i++)
    {
      point_t p = point_list->points[i];
      if(i == closest_i)
	{
	  glColor3f(1, .75, .75);
	}
      else
	glColor3f(1, 0, 0);

      glVertex3f(p.x - 1, p.y - 1, 0);
      glVertex3f(p.x + 1, p.y - 1, 0);
      glVertex3f(p.x + 1, p.y + 1, 0);
      glVertex3f(p.x - 1, p.y + 1, 0);
    }
  glEnd();
}

int load_points(point_list_t * point_list, const char * path)
{
  FILE * fp;
  int count;
  fp = fopen(path, "rb");

  if(fread(&count, sizeof(Uint32), 1, fp) != 1)
    goto error;
  point_list->count = count;

  point_list->points = malloc(sizeof(point_t) * count);
  if(fread(point_list->points, sizeof(point_t), count, fp) != count)
    {
      free(point_list->points);
      goto error;
    }

  fclose(fp);

  return 0;

 error:
  printf("Error while loading points!\n");
  fclose(fp);
  return 1;
}

int save_points(point_list_t * point_list, const char * path)
{
  FILE * fp;
  fp = fopen(path, "wb");

  if(fwrite(&point_list->count, sizeof(int), 1, fp) != 1)
    goto error;

  if(fwrite(point_list->points, sizeof(point_t), point_list->count, fp) != point_list->count)
    goto error;

  fclose(fp);
  return 0;

 error:
  printf("Error while saving points!\n");
  fclose(fp);
  return 1;
}
