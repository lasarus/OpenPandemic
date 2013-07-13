#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <float.h>

#include "point.h"
#include "triangle.h"

extern int screen_width, screen_height;

triangle_t new_triangle(point_t p1, point_t p2, point_t p3)
{
  triangle_t triangle;

  triangle.p[0] = p1;
  triangle.p[1] = p2;
  triangle.p[2] = p3;
  
  return triangle;
}

void init_triangle_list(triangle_list_t * triangle_list)
{
  triangle_list->count = 0;
  triangle_list->triangles = NULL;
}

void add_triangle(triangle_list_t * triangle_list, triangle_t triangle)
{
  triangle_list->count++;
  triangle_list->triangles = realloc(triangle_list->triangles, sizeof(triangle_t) *
			       triangle_list->count);

  triangle_list->triangles[triangle_list->count - 1] = triangle;
}

void draw_triangle_list(triangle_list_t * triangle_list, double mx, double my)
{
  int i;

  glBegin(GL_TRIANGLES);
  for(i = 0; i < triangle_list->count; i++)
    {
      triangle_t t = triangle_list->triangles[i];
      glColor4f(0, 1, 0, .2);

      glVertex3f(t.p[0].x, t.p[0].y, 0);
      glVertex3f(t.p[1].x, t.p[1].y, 0);
      glVertex3f(t.p[2].x, t.p[2].y, 0);
    }
  glEnd();
}

static void print_point(point_t p)
{
  double x, y;

  x = 1 - (p.x / (double)screen_width) * 2;
  y = 1 - (p.y / (double)screen_height) * 2;

  printf("{%f %f}", x, y);
}

void print_triangles(triangle_list_t * triangle_list)
{
  int i;

  for(i = 0; i < triangle_list->count; i++)
    {
      printf("{");

      print_point(triangle_list->triangles[i].p[0]);
      printf(" ");
      print_point(triangle_list->triangles[i].p[1]);
      printf(" ");
      print_point(triangle_list->triangles[i].p[2]);

      printf("}\n");
    }
}
