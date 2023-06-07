#ifndef CLUSTER_PSO_H
#define CLUSTER_PSO_H

#include <map>
#include <utility>
#include <vector>
#include <random>
#include <algorithm>
#include "optimizer.h"
#include "individual.h"

class Cluster_Pso : public Optimizer
{
public:
    Cluster_Pso(int size, pos gateway, pos sensor, std::vector<u_int> ids, config_t config, position base);
    ~Cluster_Pso();
    float Calculate_distance(position a, position b);

private:
    // attributes
    std::vector<float_v> velocity_;
    int gateway_size;

    void Optimize();
    fitness_t Fitness(Individual &individual);
};
#endif // CLUSTER_PSO_H