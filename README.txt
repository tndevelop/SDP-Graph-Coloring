# SDP-Graph-Coloring Program User Guide


## Compilation

A precompiled version of the program is included in the SDP-Graph-Coloring_Submission root directory called graphColoring.exe. 

Alternatively, to compile the project on a Windows operative system:

1) Import the files into a visual studio project

2) To enable the execution of the program on large graphs go to Build -> Configuration Manager and set the project Platform to be x64 (if available)

3) Go to Debug -> <Project> Debug Properties, then to Linker -> System and set Enable Large Addresses to YES (/LARGEADDRESSAWARE)

4) Build the solution (Ctrl + Shift + B)


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

## Graph list

The following is a list of the graphs and their corresponding input numbers for use with the program:

 0 manual/v10.gra
 1 manual/v100.gra
 2 manual/v1000.gra
 3 small_sparse_real/agrocyc_dag_uniq.gra
 4 small_sparse_real/human_dag_uniq.gra
 5 small_dense_real/arXiv_sub_6000-1.gra
 6 scaleFree/ba10k5d.gra
 7 large/uniprotenc_150m.scc.gra
 8 large/citeseer.scc.gra
 9 large/uniprotenc_22m.scc.gra
 10 large/go_uniprot.gra
 11 large/citeseerx.gra
 12 large/cit-Patents.scc.gra
 13 large/uniprotenc_100m.scc.gra
 14 small_sparse_real/vchocyc_dag_uniq
 15 rgg/rgg_n_2_15_s0.graph
 16 rgg/rgg_n_2_16_s0.graph
 17 rgg/rgg_n_2_17_s0.graph
 18 rgg/rgg_n_2_18_s0.graph
 19 rgg/rgg_n_2_19_s0.graph
