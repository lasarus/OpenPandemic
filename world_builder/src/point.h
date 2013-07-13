#ifndef POINT_H
#define POINT_H

typedef struct point
{
  double x, y;
} point_t;

typedef struct point_list
{
  int count;
  point_t * points;
} point_list_t;

point_t new_point(double x, double y);

void init_point_list(point_list_t * point_list);
void add_point(point_list_t * point_list, point_t point);
void remove_point(point_list_t * point_list, int i);

point_t get_closest_point(point_list_t * point_list, double x, double y);
int get_closest_point_i(point_list_t * point_list, double x, double y);

void draw_point_list(point_list_t * point_list, double mx, double my);

int load_points(point_list_t * point_list, const char * path);
int save_points(point_list_t * point_list, const char * path);

#endif
