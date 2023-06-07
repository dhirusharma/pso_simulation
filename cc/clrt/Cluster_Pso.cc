#include "Cluster_Pso.h"
#include <stdio.h>

using namespace std;

Cluster_Pso::Cluster_Pso(int size, pos gateway, pos sensor, vector<u_int> ids, config_t config, position base)
    : Optimizer(gateway, sensor, ids, config, base)

{
    gateway_size = size;
}

Cluster_Pso::~Cluster_Pso()
{
}

void Cluster_Pso::Optimize()
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

            for (unsigned int idx = 0; idx < nb_nodes_; idx++)
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
            if (individual.GetFitness().total > best_local.GetFitness().total)
                best_local = individual;
        }
    }
}

fitness_t
Cluster_Pso::Fitness(Individual &individual)
{
    auto genes = individual.GetGenes();

    float term1 = 0.0, term2 = 0.0;

    // Calculating fitness for average lifetime

    // Assigning gateway to sensor
    vector<u_int> sensor_gateway;
    int i = 0;
    for (auto gene : genes)
    {
        int size = possible_gateway[ids_[i]].size();
        int gate = ceil(gene * size);
        sensor_gateway.push_back(gate);
        i++;
    }

    // Number of sensor in gateway
    vector<u_int> number_sensor;
    for (int i = 0; i < gateway_size; i++)
    {
        int count = std::count(sensor_gateway.begin(), sensor_gateway.end(), i);
        number_sensor.push_back(count);
    }

    vector<float> gateway_energy_per_round;
    for (int i = 0; auto gene : genes)
    {
        float E_ELEC = 50e-9;
        float E_DA = 5e-9;
        float E_MP = 0.0013e-12;
        float E_FS = 10e-12;
        float THRESHOLD_DIST = sqrt(E_FS / E_MP);

        int size = possible_gateway[ids_[i]].size();
        int gate = ceil(gene * size);
        int next_hop = possible_gateway[ids_[i]][gate];
        float energy_transfer = 0;
        while (gateway_pos[next_hop] != base_station)
        {
            int size = possible_gateway[next_hop].size();
            int gate = ceil(gene * size);
            auto next_hop_index = find(ids_.begin(), ids_.end(), gate);
            int this_hop = next_hop;
            next_hop = possible_gateway[*next_hop_index][gate];
            if (Calculate_distance(gateway_pos[ids_[this_hop]], gateway_pos[ids_[next_hop]]) > THRESHOLD_DIST)
                energy_transfer += E_MP + E_ELEC;
            else
                energy_transfer += E_FS + E_ELEC;
        }
        float energy = number_sensor[i] * (E_DA + E_ELEC) + (energy_transfer);
        gateway_energy_per_round.push_back(energy);
    }

    // Each gateway lifetime = number of rounds
    vector<float> gateway_lifetime;
    for (int i = 0; i < gateway_size; i++)
    {
        gateway_lifetime.push_back(energies_[ids_[i]] / gateway_energy_per_round[i]);
    }

    // total lifetime of gateway....needed to calculate for average lifetime
    double total_lifetime_gateway = 0;
    for (long unsigned int i = 0; i < gateway_lifetime.size(); i++)
    {
        total_lifetime_gateway += gateway_lifetime[i];
    }
    double average_lifetime_gateway = total_lifetime_gateway / gateway_size;

    // Calculating standard deviation of lifetime
    double temp = 0;
    for (int i = 0; i < gateway_size; i++)
    {
        temp += pow((average_lifetime_gateway - gateway_lifetime[i]), 2);
    }

    temp = temp / gateway_size;
    temp = 1 / temp;

    term1 = temp;

    // Calculating fitness for average cluster distance
    vector<float> average_cluster_distance;
    for (int i = 0; i < gateway_size; i++)
    {
        int gateway_id = ids_[i];
        float total_dist = 0;
        for (long unsigned int j = 0; j < sensor_gateway.size(); j++)
        {
            int sensor_id = ids_[sensor_gateway[j + gateway_size]];
            if (gateway_id == sensor_id)
            {
                float dist = Calculate_distance(gateway_pos[gateway_id], sensor_pos[sensor_id]);
                if (dist < 15)
                    total_dist += dist;
            }
        }
        average_cluster_distance.push_back(total_dist);
    }

    float avg_avg_cluster_dist = 0;
    for (int i = 0; i < gateway_size; i++)
    {
        avg_avg_cluster_dist += average_cluster_distance[i];
    }
    avg_avg_cluster_dist /= gateway_size;

    // Calculating standard deviation of average cluster distance
    temp = 0;
    for (int i = 0; i < gateway_size; i++)
    {
        temp += pow((avg_avg_cluster_dist - average_cluster_distance[i]), 2);
    }

    temp = temp / gateway_size;
    temp = 1 / temp;
    term2 = temp;

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

float Cluster_Pso::Calculate_distance(position a, position b)
{
    float dist_x = pow(a.first - b.first, 2);
    float dist_y = pow(a.second = b.second, 2);
    return sqrt(dist_x + dist_y);
}