#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "vertex.h"
#include "font.h"

typedef struct character_data
{
  struct
  {
    vector_t v;
    tex_coord_t t;
  } data[4];
} character_data_t;

font_buffer_t generate_font_buffer(GLuint texture, GLuint buffer, char * str)
{
  int i;
  character_data_t * data = NULL;
  font_buffer_t font_buffer;

  font_buffer.len = strlen(str);
  font_buffer.buffer = buffer;
  font_buffer.texture = texture;

  data = malloc(sizeof(character_data_t) * font_buffer.len);

  for(i = 0; i < font_buffer.len; i++)
    {
      char c;
      int ci, cj;

      c = str[i];
      ci = c % 16;
      cj = c / 16;

      data[i].data[0].v.x = i;
      data[i].data[0].v.z = 0;
      data[i].data[0].v.y = 0;

      data[i].data[0].t.x = ci / 16.;
      data[i].data[0].t.y = cj / 16.;

      data[i].data[1].v.x = i;
      data[i].data[1].v.z = 0;
      data[i].data[1].v.y = 1;

      data[i].data[1].t.x = ci / 16.;
      data[i].data[1].t.y = (cj + 1) / 16.;

      data[i].data[2].v.x = i + 1;
      data[i].data[2].v.z = 0;
      data[i].data[2].v.y = 1;

      data[i].data[2].t.x = (ci + 1) / 16.;
      data[i].data[2].t.y = (cj + 1) / 16.;

      data[i].data[3].v.x = i + 1;
      data[i].data[3].v.z = 0;
      data[i].data[3].v.y = 0;

      data[i].data[3].t.x = (ci + 1) / 16.;
      data[i].data[3].t.y = cj / 16.;
    }

  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER,
	       sizeof(character_data_t) * font_buffer.len,
	       data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  free(data);

  return font_buffer;
}

font_buffer_t generate_font_buffer_vbo(GLuint texture, char * str)
{
  GLuint buffer;
  glGenBuffers(1, &buffer);
  return generate_font_buffer(texture, buffer, str);
}

void draw_font_buffer(font_buffer_t * font_buffer)
{
  GLuint buffer = font_buffer->buffer;
  GLuint texture = font_buffer->texture;

  glBindTexture(GL_TEXTURE_2D, texture);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);

  glVertexPointer(3, GL_FLOAT, sizeof(float) * 5, NULL);
  glTexCoordPointer(2, GL_FLOAT, sizeof(float) * 5, (void *)(sizeof(float) * 3));

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glDrawArrays(GL_QUADS, 0, font_buffer->len * 4);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
