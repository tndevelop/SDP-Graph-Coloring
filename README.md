# SDP-Graph-Coloring


# Introduction
Graph coloring is a well known computer science problem. Given a graph the problem consists in assigning each node of the graph a color which must differ from the color of his adjacent nodes, to which we will refer as neighbors.

In order to achieve that both sequential and parallel algorithms can be used with different performance and solutions obtained. Performance of an algorithm can be measured both in terms of time taken to execute the color assignment and memory used by the program; different solutions on the other hand can be optimal, i.e. use the least number of colors for the specific graph, or non-optimal. 

In this project we didn't focus on the optimality of the solution as we payed more attention to the algorithms' performance.
Of course the number of colors used should be sensitively lower than the number of nodes whenever this is possible to achieve.

Four algorithms that were taken in consideration during the implementation phase. 
1) A sequencial greedy algorithm
2) Three parallel algorithms 

The aim of this project is to prove that the parallel algorithms can outperform, in terms of time taken, the sequential one


# Commands

In order to compile the project on a Linux operative system this command must be executed: 

1) g++ -std=c++11 -g -pthread main.cpp util.cpp graphColoringAlgorithms.cpp algorithmMIS.cpp algorithmGreedy.cpp

In order to run the project on a Linux operative system there are different options:

1) ./a.out <graph> <algorithm> <number of threads> (e.g. "./a.out 0 0 4") -> select graph, algorithm and number of threads
2) ./a.out <graph> <algorithm> (e.g. "./a.out 0 0") -> select graph and algorithm and use the maximum number of threads available on the machine
2) ./a.out <graph> (e.g. "./a.out 0" ) -> select the graph, use the maximum number of threads available on the machine and be prompted which algorithm to run 
   where the graph is an integer from 0 to 13, the algorithm is an integer from 0 to 7 and the number of threads is an integer in the range 0 to infinite (the number of threads available on the machine won't be exceded anyway) 

