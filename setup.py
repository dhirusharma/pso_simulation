#!/usr/bin/env python

"""setup that compiles C++ wrapper to be used as python module.
"""

from distutils.core import setup, Extension
import os

# this project uses c++11 codes
os.environ["CC"] = "g++ -std=c++20"

swig_opts = ['-shadow', '-c++']
source_path = 'cc/'
build_path = 'build/temp.linux-x86_64-cpython-310/'+source_path

modified_pso = Extension('cc._modified_pso',
                         sources=[source_path+'modified_pso.cc',
                                  source_path+'regions.cc',
                                  source_path+'individual.cc',
                                  source_path+'optimizer.cc',
                                  source_path+'modified_pso.i'],
                         swig_opts=swig_opts)
pso = Extension('cc._pso',
                sources=[source_path+'pso.cc',
                         source_path+'pso.i'],
                extra_objects=[build_path+'regions.o',
                               build_path+'individual.o',
                               build_path+'optimizer.o'],
                swig_opts=swig_opts)
genetic_algorithm = Extension('cc._genetic_algorithm',
                              sources=[source_path+'genetic_algorithm.cc',
                                       source_path+'genetic_algorithm.i'],
                              extra_objects=[build_path+'regions.o',
                                             build_path+'individual.o',
                                             build_path+'optimizer.o'],
                              swig_opts=swig_opts)

ecca = Extension('cc._ecca',
                 sources=[source_path+'ecca.cc',
                          source_path+'ecca.i'],
                 extra_objects=[build_path+'regions.o',
                                build_path+'individual.o',
                                build_path+'optimizer.o'],
                 swig_opts=swig_opts)

Cluster_pso = Extension('cc.clrt._Cluster_Pso',
                        sources=[source_path+'clrt/'+'Cluster_Pso.cc',
                                 source_path+'clrt/'+'individual.cc',
                                 source_path+'clrt/'+'optimizer.cc',
                                 source_path+'clrt/'+'Cluster_Pso.i'],
                        swig_opts=swig_opts)

Routing_pso = Extension('cc.clrt._Routing_Pso',
                        sources=[source_path+'clrt/'+'Routing_Pso.cc',
                                 source_path+'clrt/'+'individual.cc',
                                 source_path+'clrt/'+'optimizer.cc',
                                 source_path+'clrt/'+'Routing_Pso.i'],
                        swig_opts=swig_opts)

# compile C++ libraries
setup(name='Optimizers',
      version='1.0',
      author="Dhiraj Sharma",
      description="""C++ wrappers for performance optimization.""",
      ext_modules=[modified_pso, genetic_algorithm,
                   pso, ecca, Cluster_pso, Routing_pso],
      py_modules=["modified_pso", "genetic_algorithm",
                  "pso", "ecca", "Cluster_Pso", "Routing_Pso"],
      )
