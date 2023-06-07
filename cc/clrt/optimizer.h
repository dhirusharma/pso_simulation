// The version of regions in c++ is used to improve performance. This
// code is supposed to be called by the Python script.

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <random>
#include "types.h"
#include "individual.h"

// most of these definitions are used to improve readability
typedef unsigned int u_int;
typedef std::vector<float> float_v;
typedef std::pair<float, float> position;
typedef std::map<u_int, std::vector<u_int>> possible;
typedef std::map<u_int, position> pos;
typedef std::map<u_int, float> dict_t;

typedef std::vector<float> individual_t;
typedef std::pair<std::map<std::string, u_int>,
                  std::map<std::string, float>>
    config_t;

class Optimizer
{

  friend class Individual;

public:
  Optimizer(pos gateway, pos sensor, std::vector<u_int> ids, config_t config, position base);
  Optimizer(pos gateway, pos sensor, std::vector<u_int> ids, config_t config, position base,int dimension);
  virtual ~Optimizer();

  // returns a std::vector with the best configuration found (best particle),
  // indicating, for each node, if it should sleep or not;
  // the learning trace (trace of the best fitness value at each iteration);
  // and a std::vector with the coverage and overlapping areas for the best
  // configuration
  individual_t Run(possible gateway);
  individual_t Run(dict_t energy, possible gateway, possible sensor, std::vector<float> best_rout);

  // setters & getters
  void SetAlpha(float value);
  void SetBeta(float value);
  void SetGamma(float value);
  std::vector<float> GetLearningTrace();
  std::vector<float> GetTerm1Trace();
  std::vector<float> GetTerm2Trace();

protected:
  // attributes
  std::vector<u_int> ids_;
  u_int nb_nodes_;
  u_int nb_individuals_;
  u_int max_iterations_;
  float wmax_;
  float wmin_;

  float fitness_alpha_;
  float fitness_beta_;
  float fitness_gamma_;

  // session attributes (stored here for convenience)

  // std::vector with all individuals
  std::vector<Individual> population_;
  Individual best_global_;
  std::vector<Individual> best_locals_;

  // random related
  std::default_random_engine generator_;

  dict_t energies_;
  possible possible_gateway; // Gateways in communication range of sensor
  possible backup_sensor;    // Sensor in communication range of sensor

  pos gateway_pos; // Position of all gateway
  pos sensor_pos;  // position of all sensor
  position base_station;
  std::vector<float> best_routing;
  // learning traces for the last run
  std::vector<float> learning_trace_;
  std::vector<float> term1_trace_;
  std::vector<float> term2_trace_;

  // methods
  void PrintIndividual(individual_t individual);

  void CreatePopulation();
  virtual void Optimize();

  void ClearLearningTraces();
  void InitializeSessionData(dict_t energy, possible gateway, possible sensor, std::vector<float> best_rout);
  void InitializeSessionData(possible gateway);

  void PushIntoLearningTraces(const fitness_t &fitness);

  // Returns a float indicating how fit a individual/particle is,
  // and the coverage and overlapping areas for that particle.
  virtual fitness_t Fitness(Individual &individual) = 0;
};
#endif // OPTIMIZER_H
