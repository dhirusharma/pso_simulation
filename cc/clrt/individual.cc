#include "individual.h"
#include <stdio.h>
#include "optimizer.h"

// declaration of static members
// fitness_t Individual::best_global_;
// std::vector<char> Individual::best_genes_;
Optimizer *Individual::optimizer_;
// unsigned int Individual::fresh_run_;
std::default_random_engine Individual::generator_;

Individual::Individual()
{
  optimizer_ = NULL;
  fitness_.total = 0.0;
}

Individual::Individual(unsigned int idx, Optimizer *container_handler)
{
  optimizer_ = container_handler;
  idx_ = idx;
  auto nb_genes = optimizer_->nb_nodes_;
  genes_ = std::vector<float>(nb_genes, 0);
  // days_alive_ = 0;
  SampleNewGenes();
  container_handler->Fitness(*this);
}

Individual::~Individual()
{
}

// void
// Individual::SetNewRun() {
//   best_global_.total = 0.0;
// }

// std::vector<char>
// Individual::GetBestGenes() {
//   return best_genes_;
// }
//
// fitness_t
// Individual::GetBestFitness() {
//   return best_global_;
// }

// getters & setters
fitness_t
Individual::GetFitness() const
{
  return fitness_;
}

void Individual::SetFitness(fitness_t value)
{
  fitness_ = value;
}

std::vector<float>
Individual::GetGenes()
{
  return genes_;
}

void Individual::SetGenes(std::vector<float> value)
{
  genes_ = value;
  optimizer_->Fitness(*this);
}

void Individual::SampleNewGenes()
{
  std::uniform_real_distribution<float> distribution(0.0, 1.0);
  for (unsigned int idx = 0; idx < genes_.size(); idx++)
  {
    float random = distribution(generator_);
    genes_[idx] = random;
  }
  // UpdateFitness();
}
