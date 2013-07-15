#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vertex.h"
#include "land.h"

enum
  {
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_SPACE,
    TOKEN_FLOAT,
    TOKEN_TYPE,
    TOKEN_STRING,
    TOKEN_UNKNOWN,
    TOKEN_EOF
  };

union
{
  float f;
  char * str;
} token_data;

static int matches_float(char * str)
{
  int dot = 0;
  int number = 0;

  if(*str == '-')
    str++;

  while(*str != 0)
    {
      if(*str == '.')
	{
	  if(dot)
	    return 0;
	  else
	    dot = 1;
	}
      else if(*str < '0' || *str > '9')
	return 0;

      number = 1;

      str++;
    }

  if(!number)
    return 0;

  return 1;
}

static int matches_string_start(char * str)
{
  if(*str != '\"')
    return 0;
  str++;

  while(*str != 0)
    {
      if(*str == '\"' && *(str + 1) != 0)
	return 0;
      str++;
    }

  return 1;
}

static int matches_string(char * str)
{
  if(*str != '\"' || str[strlen(str) - 1] != '\"')
    return 0;
  return 1;
}

static int matches_type_start(char * str)
{
  if(*str != ':')
    return 0;

  str++;

  while(*str != 0)
    {
      if(*str == ':' && *(str + 1) != 0)
	return 0;
      str++;
    }

  return 1;
}

static int matches_type(char * str)
{
  if(*str != ':' || str[strlen(str) - 1] != ':')
    return 0;
  return 1;
}


static int matches_single_char(char * str)
{
  if(str[1] != 0)
    return 0;

  if(str[0] == ' ' || str[0] == '{' || str[0] == '}')
    return 1;
  return 0;
}

int left_over = 0;
static int read_token(FILE * file)
{
  char buffer[256];
  int c;
  memset(buffer, 0, 256);
  buffer[0] = left_over;

  if(left_over == EOF)
    return TOKEN_EOF;

  while(1)
    {
      int len = strlen(buffer);
      c = fgetc(file);

      buffer[len] = c;
      buffer[len + 1] = 0;

      if((!matches_float(buffer) && !matches_single_char(buffer) && !matches_string_start(buffer) && !matches_type_start(buffer)) || 
	 c == EOF)
	{
	  buffer[len] = 0;

	  left_over = c;

	  if(matches_float(buffer))
	    {
	      token_data.f = atof(buffer);
	      return TOKEN_FLOAT;
	    }
	  else if(matches_single_char(buffer))
	    {
	      if(buffer[0] == ' ')
		return TOKEN_SPACE;
	      else if(buffer[0] == '{')
		return TOKEN_LBRACKET;
	      else if(buffer[0] == '}')
		return TOKEN_RBRACKET;
	    }
	  else if(matches_string(buffer))
	    {
	      token_data.str = strndup(buffer + 1, strlen(buffer) - 2);
	      return TOKEN_STRING;
	    }
	  else if(matches_type(buffer))
	    {
	      token_data.str = strndup(buffer + 1, strlen(buffer) - 2);
	      return TOKEN_TYPE;
	    }
	  else
	    return TOKEN_UNKNOWN;
	}
    }
}

static void read_color(FILE * file, float * r, float * g, float * b)
{
  int token;
  int stage = 0;

  /* STAGE 0: search for color array */
  /* STAGE 1: read color array floats */
  int current_color = 0;

  while(1)
    {
      token = read_token(file);

      if(token == TOKEN_EOF)
	return;

      if(stage == 0)
	{
	  if(token == TOKEN_LBRACKET)
	    {
	      stage = 1;
	    }
	}
      else if(stage == 1)
	{
	  if(token == TOKEN_FLOAT)
	    {
	      float * c = NULL;
	      switch(current_color)
		{
		case 0: c = r; break;
		case 1: c = g; break;
		case 2: c = b; break;
		}

	      *c = token_data.f;
	      current_color++;
	    }
	  else if(token == TOKEN_RBRACKET)
	    {
	      break;
	    }
	}
    }
}

void load_landmass(landmass_t * landmass, FILE * file)
{
  int token;
  int stage = 0;

  /* STAGE 0: read countries */
  /* STAGE 1: read country triangles */
  /* STAGE 2: read vertices in triangles */
  /* STAGE 3: read floats in vertices */
  int current_country = 0;
  int current_triangle = 0;
  int current_vertex = 0;
  int current_pair = 0;

  float c_r = 0, c_g = 1, c_b = 0;

  landmass->count = 0;
  landmass->countries = NULL;

  while(1)
    {
      token = read_token(file);

      if(token == TOKEN_EOF)
	break;

      if(stage == 0)
	{
	  if(token == TOKEN_LBRACKET)
	    {
	      stage++;
	      current_triangle = 0;

	      c_r = 0;
	      c_g = 1;
	      c_b = 0;

	      landmass->count++;
	      landmass->countries = realloc(landmass->countries,
					    sizeof(country_t) * landmass->count);
	      landmass->countries[landmass->count - 1].count = 0;
	      landmass->countries[landmass->count - 1].triangles = NULL;
	      landmass->countries[landmass->count - 1].selectable = 1;
	    }
	  else if(token == TOKEN_RBRACKET)
	    {
	      current_country++;
	      stage--;
	    }
	  else
	    continue;
	}
      else if(stage == 1)
	{
	  if(token == TOKEN_LBRACKET)
	    {
	      stage++;
	      landmass->countries[current_country].count++;
      
	      landmass->countries[current_country].triangles =
		realloc(landmass->countries[current_country].triangles, sizeof(c_triangle_t) * landmass->countries[current_country].count);

	      current_vertex = 0;
	      landmass->countries[current_country].triangles[current_triangle].color.x = c_r;
	      landmass->countries[current_country].triangles[current_triangle].color.y = c_g;
	      landmass->countries[current_country].triangles[current_triangle].color.z = c_b;
	    }
	  else if(token == TOKEN_TYPE)
	    {
	      char * str = token_data.str;
	      if(strcmp(str, "name") == 0)
		{
		  int ntoken = read_token(file);
		  if(ntoken == TOKEN_STRING)
		    {
		      landmass->countries[current_country].name = token_data.str;
		      printf("Name: %s\n", token_data.str);
		    }
		}
	      else if(strcmp(str, "population") == 0)
		{
		  int ntoken = read_token(file);
		  if(ntoken == TOKEN_FLOAT)
		    {
		      landmass->countries[current_country].population = (int)token_data.f;
		      printf("Population: %i\n", (int)token_data.f);
		    }
		}
	      else if(strcmp(str, "color") == 0)
		{
		  float r, g, b;
		  read_color(file, &r, &g, &b);

		  c_r = r;
		  c_g = g;
		  c_b = b;
		}
	      else if(strcmp(str, "selectable") == 0)
		{
		  int ntoken = read_token(file);
		  if(ntoken == TOKEN_FLOAT)
		    {
		      landmass->countries[current_country].selectable = (int)token_data.f;
		    }
		}
	      free(str);
	    }
	  else if(token == TOKEN_RBRACKET)
	    {
	      current_country++;
	      stage--;
	    }
	  else
	    continue;
	}
      else if(stage == 2)
	{
	  if(token == TOKEN_LBRACKET)
	    {
	      stage++;
	      current_pair = 0;
	    }
	  else if(token == TOKEN_RBRACKET)
	    {
	      stage--;
	      current_triangle++;
	    }
	  else
	    continue;
	}
      else if(stage == 3)
	{
	  if(token == TOKEN_FLOAT)
	    {
	      s_vertex_t * vertex;

	      vertex = &landmass->countries[current_country].triangles[current_triangle].v[current_vertex];
	      *((current_pair) ? &vertex->ring : &vertex->sector) = token_data.f;
	      current_pair++;
	    }
	  else if(token == TOKEN_RBRACKET)
	    {
	      stage--;
	      current_vertex++;
	    }
	  else
	    continue;
	}
    }
}
