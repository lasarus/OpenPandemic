#ifndef FONT_H
#define FONT_H

typedef struct font_buffer
{
  GLuint texture, buffer;
  char * str;
  int len;
} font_buffer_t;

font_buffer_t generate_font_buffer_vbo(GLuint texture, char * str);
font_buffer_t generate_font_buffer(GLuint texture, GLuint buffer, char * str);
void draw_font_buffer(font_buffer_t * font_buffer);

#endif
