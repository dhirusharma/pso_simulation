import numpy as np
import logging
from time import time

import config as cf
from python.utils.grid import *
from python.utils.regions_converter import *
from python.utils.utils import *
from cc.clrt.Routing_Pso import *
from cc.clrt.Cluster_Pso import *

from multiprocessing.dummy import Pool as ThreadPool

"""Wraps the C++ instance that executes the PSO and also calculates
all coverage information.
"""


class Routing(object):

    def __init__(self, network, optimizer_class):

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
        possible_gateway = {}
        for gateway in self._network.get_alive_gateway():
            possible_gateways = [gateways.id for gateways in self._network.get_alive_gateway()
                                 if calculate_distance(gateway, gateways) < cf.COVERAGE_RADIUS
                                 and calculate_distance(gateway, gateways) != 0]
            if calculate_distance(gateway, self._network.get_BS()) < cf.COVERAGE_RADIUS and calculate_distance(gateway, self._network.get_BS()) != 0:
                possible_gateways.append(self._network.get_BS().id)
            possible_gateway[gateway.id] = possible_gateways

        logging.info("inside routing schedule")
        best_configuration = self._optimizer.Run(possible_gateway)
        learning_trace = self._optimizer.GetLearningTrace()
        term1_trace = self._optimizer.GetTerm1Trace()
        term2_trace = self._optimizer.GetTerm2Trace()

        gateway = self._network.get_alive_gateway()
        for i in range(len(gateway)):
            id = best_configuration[i] * len(possible_gateway[i])
            gateway[i]._next_hop = math.ceil(id)

        log = {}
        log['best_configuration'] = best_configuration
        log['initial_fitness'] = learning_trace[0]
        log['final_fitness'] = learning_trace[-1]
        log['term1_initial'] = term1_trace[0]
        log['term1_final'] = term1_trace[-1]
        log['term2_initial'] = term2_trace[0]
        log['term2_final'] = term2_trace[-1]

        return log
