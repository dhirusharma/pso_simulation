#include "optimizer.h"
#include <stdio.h>

using namespace std;

// public methods

Optimizer::Optimizer(pos gateway, pos sensor, vector<u_int> ids, config_t config, position base)
    : ids_(ids)
{

  gateway_pos = gateway;
  sensor_pos = sensor;
  // config.first are all integers
  nb_individuals_ = config.first["NB_INDIVIDUALS"];
  max_iterations_ = config.first["MAX_ITERATIONS"];
  // config.second are all floats
  fitness_alpha_ = config.second["FITNESS_ALPHA"];
  fitness_beta_ = config.second["FITNESS_BETA"];
  fitness_gamma_ = config.second["FITNESS_GAMMA"];
  wmax_ = config.second["WMAX"];
  wmin_ = config.second["WMIN"];

  base_station = base;
  nb_nodes_ = ids.size();
}

Optimizer::Optimizer(pos gateway, pos sensor, vector<u_int> ids, config_t config, position base, int dimension)
    : ids_(ids), nb_nodes_(dimension)
{

  gateway_pos = gateway;
  sensor_pos = sensor;
  // config.first are all integers
  nb_individuals_ = config.first["NB_INDIVIDUALS"];
  max_iterations_ = config.first["MAX_ITERATIONS"];
  // config.second are all floats
  fitness_alpha_ = config.second["FITNESS_ALPHA"];
  fitness_beta_ = config.second["FITNESS_BETA"];
  fitness_gamma_ = config.second["FITNESS_GAMMA"];
  wmax_ = config.second["WMAX"];
  wmin_ = config.second["WMIN"];

  base_station = base;
}

Optimizer::~Optimizer()
{
}

individual_t
Optimizer::Run(dict_t energy, possible gateway, possible sensor, vector<float> best_rout)
{
  ClearLearningTraces();
  InitializeSessionData(energy, gateway, sensor, best_rout);

  // depleted nodes should not be taken into consideration

  CreatePopulation();
  Optimize();
  return best_global_.GetGenes();
}

individual_t
Optimizer::Run(possible gateway)
{
  ClearLearningTraces();
  InitializeSessionData(gateway);

  // depleted nodes should not be taken into consideration

  CreatePopulation();
  Optimize();
  return best_global_.GetGenes();
}

void Optimizer::Optimize()
{
}

void Optimizer::CreatePopulation()
{
  population_.clear();
  best_locals_.clear();
  population_.push_back(Individual(0, this));
  best_global_ = population_[0];
  best_locals_.push_back(population_[0]);
  for (unsigned int idx = 1; idx < nb_individuals_; idx++)
  {
    population_.push_back(Individual(idx, this));
    best_locals_.push_back(population_[idx]);
  }
}

void Optimizer::PrintIndividual(individual_t individual)
{
  for (auto const &gene : individual)
  {
    printf("%f", gene);
  }
  printf("\n");
}

// setters & getters
void Optimizer::SetAlpha(float value)
{
  fitness_alpha_ = value;
}

void Optimizer::SetBeta(float value)
{
  fitness_beta_ = value;
}

void Optimizer::SetGamma(float value)
{
  fitness_gamma_ = value;
}

vector<float>
Optimizer::GetLearningTrace()
{
  return learning_trace_;
}

vector<float>
Optimizer::GetTerm1Trace()
{
  return term1_trace_;
}

vector<float>
Optimizer::GetTerm2Trace()
{
  return term2_trace_;
}

void Optimizer::ClearLearningTraces()
{
  learning_trace_.clear();
  term1_trace_.clear();
  term2_trace_.clear();
}

void Optimizer::InitializeSessionData(dict_t energy, possible gateway, possible sensor, vector<float> best_rout)
{
  energies_ = energy;
  possible_gateway = gateway;
  backup_sensor = sensor;
  best_routing = best_rout;
}
void Optimizer::InitializeSessionData(possible gateway)
{
  possible_gateway = gateway;
}

void Optimizer::PushIntoLearningTraces(const fitness_t &fitness)
{
  learning_trace_.push_back(fitness.total);
  term1_trace_.push_back(fitness.term1);
  term2_trace_.push_back(fitness.term2);
}
