# SDP-Graph-Coloring Project

Date: 2 September 2021

Authors: 
Kylie Bedwell s287581,
Tommaso Natta s282478,
Marco Pappalardo s277867

# Introduction
Graph coloring is a well known computer science problem. Given a graph, the problem consists in assigning each node of the graph a color which differs from the color of its adjacent nodes, which we will refer to as its neighbors.

Sequential and parallel algorithms have been developed to solve the graph coloring problem. With some algorithms designed primarily for speed, and others designed to reduce the number of colors required to completely color the graph. Performance of an algorithm can be measured both in terms of time taken to execute the color assignment and memory used by the program. 

The main goal of this project was to analyze and optimize the performance of each algorithm in terms of speed. However the number of colors used by each algorithm has also been reported for the comparisons. The number of colors used to color the graph should be lower than the number of nodes, whenever this is possible to achieve.

Five graph coloring algorithms were considered during the implementation phase of this project, as described in [1]. 
1) A sequencial greedy algorithm 
2) The Jones-Plassman parallel algorithm 
3) The Largest-Degree-First parallel algorithm 
4) The Smallest-Degree-Last parallel algorithm
5) The Maximum Independent Set parallel algorithm

The aim of this project was to prove that the parallel algorithms can outperform sequential algorithms in terms of the time taken to analyze large graphs. To this end a variety of different of graphs with different sizes have been analysed, along with the effect of differing the number of available threads for the parallel algorithms.

The results of the first three algorithms have been reported in the evaluation section of this document. The last two algorithms unfortunately did not perform as well as expected and further analysis and optimisation would be required before assessing those algorithms.

# Design Choices

## Structure

The graph colouring program used the main.cpp as the starting point for the program, and includes a number of other .cpp and .h files for separating out the different functionalities of the program. 

The reading of the graph data is handled by the util.cpp and util.h files.

Each algorithm is contained in separate .cpp and .h files, which are included in the main.cpp to run the algorithms as required. 

Initially the algorithms were written together in one .cpp file, with the intention of sharing useful functions. However during the implementation of the algorithms it was soon apparent that each algorithm had its own unique way of completing the difference phases of graph coloring, and these subtle differences led to the decision to separate the algorithms to ensure they remained consistent with their definition. It also assisted the group in being able to work more easily in a concurrent manner.


## Graph Reading

The first step before any algorithm execution is to acquire data and configure them into the right form. The graph was acquired by a std::map associating in each entry the node number to a std::list of node numbers representing the neighbors of the given node.

The read step is also in charge of computing some of the auxiliary structures that the algorithms will later use, such as maps containing the degree of the node or random numbers associated to each node. 

Since this operation involves potentially many I/O operations, it was not accounted for in the algorithms' time performance, as it would skew the comparisons.    

## Parallelisation

Parallelisation was achieved through the use of the c++ thread library. The number of threads utilised by the program can be given as an input to the program, but is also limited to the maximum number of available threads on the system that is executing the program.

Synchronization of the threads was achieved using a combination of mutexes and condition variables. These were used as required when threads needed to modify shared variables, and to synchronize the threads for continuing to the next phase of algorithms when necessary.

# Experimental Evaluation

The analysis of the different graph coloring algorithms was conducted using a range of different graphs. The result for the execution time of each algorithm for each graph is shown in Table 1. The parallel algorithms were executed using from 1 to 8 threads for comparison. It can be seen that the greedy algorithm is faster than the parallel algorithms for smaller graphs. This was expected because parallel algorithms require an overhead of code to set up and synchronize the execution of the parallel threads, and the benefits of this overhead are not realised for smaller graphs. For some of the larger graphs however it can be seen that the parallel algorithms are beneficial to the program execution time, and the utilisation of a greater number of threads would likely improve the execution time further (but this analysis was not possible to the the limitations of the machines used in this analysis).

Low-density graphs benefit from the use of the parallel algorithms as well. Indeed it can be seen that on sparse graphs such as "human_dag_uniq.gra" and "agrocyc_dag_uniq.gra", even though small, the parallel algorithms perform as good as the greedy one. That means they are fast enough to compensate the overhead due to threads set up an syncronization.   

Table 1: Algorithm execution times. 

![Algorithm Execution Time](/figures/ExecutionTime.png)

The memory usage of the different algorithms was also analysed and is shown in Table 2. All algorithms use a similar amount of peak memory to analyse the various graphs, with only small differences records between the different algorithms.

Table 2: Memory usage.

![Memory Usage](/figures/MemoryUsage.png)

Finally the number of colors assigned to the graph by each algorithm was also assessed and is shown in Table 3. For some graphs, such as the citeseer.scc.gra graph, the greedy algorithm assigned the least amount of colors. However for other graphs, such as the arXiv_sub_6000-1.gra graph, the least colors was achieved by the Largest-Degree-First algorithm. 

Table 3: Number of colors used.

![Number of Colors](/figures/NumberColors.png)


# References

[1] J. R. Allwright, R. Bordawekar, P. Coddington, K. Dinçer, Christine Martin, “A Comparison of Parallel Graph Coloring Algorithms”, 1995

