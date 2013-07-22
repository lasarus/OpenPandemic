#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "vertex.h"
#include "mapping.h"
#include "land.h"
#include "disease.h"

disease_t new_disease(int type)
{
  disease_t disease;

  disease.type = type;
  disease.drug_resistance = 0;

  return disease;
}

void init_disease_simulation(disease_simulation_t * disease_simulation, disease_t disease)
{
  disease_simulation->disease = disease;
  disease_simulation->hour = 0;
  disease_simulation->start_year = 2006;
}

void simulate_step(disease_simulation_t * disease_simulation, landmass_t * landmass)
{
  disease_simulation->hour += 2;
}

int month_array[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
int month_leap_array[12] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};

char * month_string[12] =
{
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
};

void simulation_get_time(disease_simulation_t * disease_simulation, int * year, int * month, int * day, int * hour)
{
  int i;
  int h, d, y; /* hour day year */
  y = disease_simulation->start_year;
  d = disease_simulation->hour / 24;
  h = disease_simulation->hour % 24;

  *month = 0;
  while(d != 0)
    {
      if(d >= ((y % 4 == 0) ? 366 : 365))
	{
	  if(y % 4 == 0)
	    {
	      d -= 365;
	      y++;
	    }
	  else
	    {
	      d -= 364;
	      y++;
	    }
	}
      else
	{
	  int * array = (y % 4 == 0) ? month_leap_array : month_array;
	  for(i = 11; i >= 0; i--)
	    {
	      if(array[i] < d)
		{
		  d -= array[i];
		  *month = i;
		  break;
		}
	    }
	  break;
	}
    }

  *year = y;
  *day = d;
  *hour = h;
}

void simulation_get_time_string(disease_simulation_t * disease_simulation, char * buffer)
{
  int year, day, month, hour;
  simulation_get_time(disease_simulation, &year, &month, &day, &hour);
  sprintf(buffer, "%i. %s %i %.2i:00", day, month_string[month], year, hour);
}
