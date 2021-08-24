#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "util.h"
#include "graphColoringAlgorithms.h"

using namespace std;

//first parameter determines the graph that will be used as input
int main(int argc, char ** argv) {
    int maxColUsed = -1;
    string basePath = "./graphs/benchmark/";
    string graphPaths [] = {"manual/v10.gra", "manual/v100.gra", "small_sparse_real/agrocyc_dag_uniq.gra", "small_sparse_real/human_dag_uniq.gra", "small_dense_real/arXiv_sub_6000-1.gra", "scaleFree/ba10k5d.gra",
            // the next files are too large for git, need to import the "large" folder under "benchmark". It is already ignored in the .gitignore file
                            "large/uniprotenc_22m.scc.gra", "large/go_uniprot.gra" };
    string finalPath = basePath + graphPaths[atoi(argv[1])];

    map<int, list<int>> graph = readGraph(finalPath);

    vector<int> colors = initializeLabels(graph.size());

    vector<int> colorsGreedy = greedyAssignment(graph, colors, &maxColUsed);

    //some output just to be sure the application ran properly
    cout << "number of nodes: " << graph.size() << endl;
    cout << "number of greedy colors: " << maxColUsed + 1 << endl;
    cout << "for instance color " << colorsGreedy[maxColUsed] << " was assigned to node " << maxColUsed << endl; //should never be -1

    maxColUsed = -1;
    vector<int> colorsJPS = jonesPlassmannSequentialAssignment(graph, colors, &maxColUsed);

    //some output just to be sure the application ran properly
    cout << "number of JP colors: " << maxColUsed + 1 << endl;
    cout << "for instance color " << colorsJPS[maxColUsed] << " was assigned to node " << maxColUsed << endl; //should never be -1

    maxColUsed = -1;
    vector<int> colorsJP = jonesPlassmannParallelAssignment(graph, colors, &maxColUsed);

    //some output just to be sure the application ran properly
    cout << "number of JP colors: " << maxColUsed + 1 << endl;
    cout << "for instance color " << colorsJP[maxColUsed] << " was assigned to node " << maxColUsed << endl; //should never be -1

    return 0;
}
