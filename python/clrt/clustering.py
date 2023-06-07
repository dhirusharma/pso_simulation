import numpy as np
import logging
from time import time

import config as cf
from python.utils.grid import *
from python.utils.regions_converter import *
from python.utils.utils import *
from cc.clrt.Cluster_Pso import *
from cc.clrt.Routing_Pso import *

from multiprocessing.dummy import Pool as ThreadPool

"""Wraps the C++ instance that executes the PSO and also calculates
all coverage information.
"""


class Clustering(object):

    def __init__(self, network, best_rout, optimizer_class):

        self._network = network
        config_int = {'NB_INDIVIDUALS': cf.NB_INDIVIDUALS,
                      'MAX_ITERATIONS': cf.MAX_ITERATIONS}
        config_float = {'FITNESS_ALPHA': cf.FITNESS_ALPHA,
                        'FITNESS_BETA': cf.FITNESS_BETA,
                        'FITNESS_GAMMA': cf.FITNESS_GAMMA,
                        'WMAX': cf.WMAX,
                        'WMIN': cf.WMIN}

        configuration = (config_int, config_float)
        ids = [node.id for node in network.get_alive_sensor()]

        self.best_rout = best_rout

        gateway = self._network.get_alive_gateway()
        gateway_pos = {node.id: (node.pos_x, node.pos_y)
                       for node in network.get_alive_gateway()}
        sensor_pos = {node.id: (node.pos_x, node.pos_y)
                      for node in network.get_alive_sensor()}
        base_station = network.get_BS()
        base_station_pos = (base_station.pos_x, base_station.pos_y)
        self._optimizer = optimizer_class(
            len(gateway), gateway_pos, sensor_pos, ids, configuration, base_station_pos)

    def schedule(self):
        """Runs PSO to decide which nodes in the cluster will sleep. The cur-
        rent cluster head should not be put to sleep, otherwise all informa-
        tion for that node is lost.
        """
        # when a single node (CH) is alive you must keep it awake
        if (self._cluster.count_alive_nodes() <= 1):
            return {}
        membership = self._cluster[0].membership
        logging.debug("Forming Cluster %d" % (membership))

        energies = [
            node.energy_source.energy for node in self._network.get_alive_nodes()]

        sensor = self._network.get_alive_sensor()

        possible_gateway = {}

        possible_sensor = {}

        for sensor in self._network.get_alive_sensor():
            possible_sensors = [gateway.id for gateway in self._network.get_alive_sensor(
            ) if calculate_distance(sensor, gateway) < cf.COVERAGE_RADIUS and calculate_distance(sensor, gateway) != 0]
            possible_sensor[sensor.id] = possible_sensors

        possible_gateway = {}
        for gateway in self._network.get_alive_gateway():
            possible_gateways = [gateways.id for gateways in self._network.get_alive_gateway()
                                 if calculate_distance(gateway, gateways) < cf.COVERAGE_RADIUS
                                 and calculate_distance(gateway, gateways) != 0]
            possible_gateway[gateway.id] = possible_gateways

        best_configuration = self._optimizer.Run(
            energies, possible_gateway, possible_sensor, self.best_rout)
        learning_trace = self._optimizer.GetLearningTrace()
        term1_trace = self._optimizer.GetTerm1Trace()
        term2_trace = self._optimizer.GetTerm2Trace()

        sensor = self._network.get_alive_sensor()
        # gateway = self._network.get_alive_gateway()
        for i in range(len(sensor)):
            gate_id = best_configuration[i] * len(possible_gateway[i])
            gate_id = math.ceil(gate_id)
            sensor[i].membership = gate_id
            sensor[i].head_assigned = True
            sensor[i]._next_hop = gate_id

        log = {}
        log['best_configuration'] = best_configuration
        log['initial_fitness'] = learning_trace[0]
        log['final_fitness'] = learning_trace[-1]
        log['term1_initial'] = term1_trace[0]
        log['term1_final'] = term1_trace[-1]
        log['term2_initial'] = term2_trace[0]
        log['term2_final'] = term2_trace[-1]

        return log
