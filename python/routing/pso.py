
# import skfuzzy
import numpy as np
# import logging, sys

# from python.routing.mte import *
from python.utils.utils import *
from python.network.node import *
from python.network.network import Network
from python.routing.routing_protocol import *
from python.clrt.clustering import *
from python.clrt.routing import *
# from python.sleep_scheduling.sleep_scheduler import *
import config as cf

"""Every node communicate its position to the base station. Then the 
BS uses PSO to define clusters and broadcast this information to the
network. Finally, a round is executed.
"""


class PSO(RoutingProtocol):
    def __init__(self, network):
        logging.info('Entered PSO routing')
        for node in network[0:cf.NB_CLUSTERS]:
            node.energy_source.gateway()
            node.is_gateway = True

#   #def _initial_setup(self, network):
    def setup_phase(self, network, round_nb=None):
        """Clustering and Routing using PSO
        """
        logging.info('PSO: setup phase')

        rout = Routing(network, eval('Routing_Pso'))
        log_routing = rout.schedule()
        best_rout = log_routing['best_configuration']
        cluster = Clustering(network, best_rout, eval('Cluster_Pso'))
        log_cluster = cluster.schedule()
