#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <vector>
#include <random>
#include "types.h"

class Optimizer;

class Individual
{
public:
  Individual();
  Individual(unsigned int idx, Optimizer *container_handler);
  ~Individual();

  fitness_t GetFitness() const;
  void SetFitness(fitness_t value);
  std::vector<float> GetGenes();
  void SetGenes(std::vector<float> value);
  // static void SetNewRun();
  // static std::vector<char> GetBestGenes();
  // static fitness_t GetBestFitness();

  fitness_t fitness_;
  u_int idx_;

  void SampleNewGenes();
  // best fitness and genes in family lineage
  // fitness_t best_fitness_;
  // std::vector<char> best_genes;

  // best fitness and genes in population's history
  // static fitness_t best_global_fitness_;
  // static std::vector<char> best_global_genes_;

private:
  std::vector<float> genes_;

  // All individuals share a handler to optimizer
  static Optimizer *optimizer_;

  static std::default_random_engine generator_;

  // fitness_t UpdateFitness();
};
#endif // INDIVIDUAL_H
