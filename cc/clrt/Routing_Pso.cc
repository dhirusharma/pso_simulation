#include "Routing_Pso.h"
#include <stdio.h>
#include <algorithm>

using namespace std;

Routing_Pso::Routing_Pso(int size, pos gateway, pos sensor, vector<u_int> ids, config_t config, position base)
    : Optimizer(gateway, sensor, ids, config, base, size)
{
    gateway_size = size;
}

Routing_Pso::~Routing_Pso()
{
}

void Routing_Pso::Optimize()
{
    uniform_real_distribution<float> distribution(0.0, 1.0);
    uniform_real_distribution<float> distribution2(-0.5, 0.5);

    // initialize velocity_
    velocity_ = vector<float_v>(nb_individuals_, float_v(nb_nodes_, 0.0));

    for (u_int individual_idx = 0; individual_idx < nb_individuals_; individual_idx++)
        for (u_int idx = 0; idx < nb_nodes_; idx++)
            velocity_[individual_idx][idx] = distribution2(generator_);

    float acceleration = 1.0, phi1 = 2.0, phi2 = 2.0;
    for (u_int it = 0; it < max_iterations_; it++)
    {
        PushIntoLearningTraces(best_global_.GetFitness());
        for (u_int individual_idx = 0; individual_idx < nb_individuals_; individual_idx++)
        {
            Individual &individual = population_[individual_idx];
            Individual &best_local = best_locals_[individual_idx];
            auto genes = individual.GetGenes();

            for (u_int idx = 0; idx < nb_nodes_; idx++)
            {
                float r1 = distribution(generator_);
                float r2 = distribution(generator_);

                int diff_to_global = best_global_.GetGenes()[idx] - genes[idx];
                int diff_to_local = best_local.GetGenes()[idx] - genes[idx];
                velocity_[individual_idx][idx] = acceleration * velocity_[individual_idx][idx] +
                                                 phi1 * r1 * diff_to_global +
                                                 phi2 * r2 * diff_to_local;

                float velocity_norm = 1 / (1 + exp(-velocity_[individual_idx][idx]));

                genes[idx] = velocity_norm;
            }
            individual.SetGenes(genes);
            if (individual.GetFitness().total < best_local.GetFitness().total)
                best_local = individual;
        }
    }
}

fitness_t
Routing_Pso::Fitness(Individual &individual)
{
    auto genes = individual.GetGenes();

    float term1 = 0.0, term2 = 0.0;

    // Calculating Max distance
    int i = 0;
    for (auto gene : genes)
    {
        int size = possible_gateway[ids_[i]].size();
        int gate = ceil(gene * size);
        int next_hop = possible_gateway[ids_[i]][gate];
        term1 += Calculate_distance(gateway_pos[ids_[i]], gateway_pos[next_hop]);
        i++;
    }

    // Calculating Max_hop
    i = 0;
    for (auto gene : genes)
    {
        int size = possible_gateway[ids_[i]].size();
        int gate = ceil(gene * size);
        int next_hop = possible_gateway[ids_[i]][gate];
        int hops = 1;
        while (gateway_pos[next_hop] != base_station)
        {
            int size = possible_gateway[next_hop].size();
            int gate = ceil(gene * size);
            auto next_hop_index = find(ids_.begin(), ids_.begin() + 20, gate);
            next_hop = possible_gateway[*next_hop_index][gate];
            hops++;
        }
        term2 += hops;
    }

    float fitness_val = fitness_alpha_ * term1 +
                        fitness_beta_ * term2;

    fitness_t fitness_ret = {.total = fitness_val,
                             .term1 = term1,
                             .term2 = term2};

    individual.SetFitness(fitness_ret);

    if (fitness_ret.total > best_global_.GetFitness().total)
        best_global_ = individual;

    return fitness_ret;
}

float Routing_Pso::Calculate_distance(position a, position b)
{
    float dist_x = pow(a.first - b.first, 2);
    float dist_y = pow(a.second = b.second, 2);
    return sqrt(dist_x + dist_y);
}