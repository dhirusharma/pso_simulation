#include<vector>

#ifndef TYPES_H
#define TYPES_H

typedef struct {
  float total;
  float term1;
  float term2;
} fitness_t;

typedef std::vector<fitness_t> population_fitness_t;

#endif // TYPES_H
