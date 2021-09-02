# SDP-Graph-Coloring Program User Guide


## Compilation

To compile the project on a Linux operative system execute this command from a terminal at the project directory:

1) g++ -std=c++11 -g -pthread main.cpp util.cpp algorithmMIS.cpp algorithmGreedy.cpp algorithmJP.cpp algorithmSDL.cpp

This should produce an a.out file.

To compile the project on a Windows operative system:

1) Build the project from visual studio

2) Execute this command from a developer command prompt (visual studio is required) at the project directory:
 cl /EHsc main.cpp util.cpp algorithmMIS.cpp algorithmGreedy.cpp algorithmJP.cpp algorithmSDL.cpp /link /out:graphColoring.exe

This should produce an graphColoring.exe  file.

## Execution

There are multiple modes of execution for the graph coloring program. The different options are:

1) Run the program in menu mode. Sending as an argument the number of the graph to analyse. The program will then ask the user which algorithm to run, and if applicable allow the user to set the number of threads to use.  In a terminal:

	graphColoring.exe <graph>
	where the graph is an integer from 0 to 13, the algorithm is an integer from 0 to 7 and the number of threads is an integer in the range 0 to infinite (noting that the value will automatically be limited to the the number of threads available on the machine)

2) Set the graph, algorithm and number of threads inputs as arguments to the program, and the program will proceed to run the selected configuration, i.e. in a terminal:

     graphColoring.exe <graph> <algorithm> <number of threads>

     	For example: "graphColoring.exe 0 2 4" -> selects graph number 0, algorithm 2 (the Jones-Plassman algorithm) and sets number of threads to 4

3) Set the graph and algorithm as arguments to the program, and have the system use the maximum number of threads available on the machine, i.e. in a terminal:

     graphColoring.exe <graph> <algorithm>

     For example: "graphColoring.exe 0 2" -> selects graph number 0 and algorithm 2 and uses the maximum number of threads available on the machine
