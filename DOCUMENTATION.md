# SDP-Graph-Coloring Project

Date: 2 September 2021

Authors: 
Kylie Bedwell s287581,
Tomasso Natta s282478,
Marco Pappalardo s277867

# Introduction
Graph coloring is a well known computer science problem. Given a graph, the problem consists in assigning each node of the graph a color which differs from the color of its adjacent nodes, which we will refer to as its neighbors.

Sequential and parallel algorithms have been developed to solve the graph coloring problem. With some algorithms designed primarily for speed, and others designed to reduce the number of colors required to completely color the graph. Performance of an algorithm can be measured both in terms of time taken to execute the color assignment and memory used by the program. 

The main goal of this project was to analyze and optimize the performance of each algorithm in terms of speed. However the number of colors used by each algorithm has also been reported for the comparisons. The number of colors used to color the graph should be lower than the number of nodes, whenever this is possible to achieve.

Four graph coloring algorithms were considered during the implementation phase of this project, as described in [1]. 
1) A sequencial greedy algorithm 
2) The Jones-Plassman parallel algorithm 
3) The Smallest-Degree-Last parallel algorithm 
4) 

The aim of this project was to prove that the parallel algorithms can outperform sequential algorithms in terms of the time taken to analyze large graphs. To this end a variety of different of graphs with different sizes have been analysed, along with the effect of differing the number of available threads for the parallel algorithms.


# Design Choices

## Structure

The graph colouring program used the main.cpp as the starting point for the program, and includes a number of other .cpp and .h files for separating out the different functionalities of the program. 

The reading of the graph data is handled by the util.cpp and util.h files.

Each algorithm is contained in separate .cpp and .h files, which are included in the main.cpp to run the algorithms as required. 

Initially the algorithms were written together in one .cpp file, with the intention of sharing useful functions. However during the implementation of the algorithms it was soon apparent that each algorithm had its own unique way of completing the difference phases of graph coloring, and these subtle differences led to the decision to separate the algorithms to ensure they remained consistent with their definition. It also assisted the group in being able to work more easily in a concurrent manner.


## Graph Reading

%TODO - Tomasso

## Parallelisation

Parallelisation was achieved through the use of the c++ thread library. The number of threads utilised by the program can be given as an input to the program, but is also limited to the maximum number of available threads on the system that is executing the program.

Synchronization of the threads was achieved using a combination of mutexes and condition variables. These were used as required when threads needed to modify shared variables, and to synchronize the threads for continuing to the next phase of algorithms when necessary.

# Experimental Evaluation

- include memory and elapsed time for each algorithm for different sized graphs and number of threads

# Summary

# References

[1] J. R. Allwright, R. Bordawekar, P. Coddington, K. Dinçer, Christine Martin, “A Comparison of Parallel Graph Coloring Algorithms”, 1995

