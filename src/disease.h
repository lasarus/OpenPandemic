#ifndef DISEASE_H
#define DISEASE_H

enum
  {
    DISEASE_VIRUS,
    DISEASE_BACTERIA,
    DISEASE_PARASITE
  };

typedef struct disease
{
  int type;

  int drug_resistance;
} disease_t;

typedef struct disease_simulation
{
  disease_t disease;

  int hour;
  int start_year;
} disease_simulation_t;

disease_t new_disease(int type);
void init_disease_simulation(disease_simulation_t * disease_simulation, disease_t disease);
void simulate_step(disease_simulation_t * disease_simulation, landmass_t * landmass);

void simulation_get_time(disease_simulation_t * disease_simulation, int * year, int * month, int * day, int * hour);
void simulation_get_time_string(disease_simulation_t * disease_simulation, char * buffer);

#endif
