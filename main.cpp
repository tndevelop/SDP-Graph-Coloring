#include "util.h"
#include "algorithmMIS.h"
#include "algorithmGreedy.h"
#include "algorithmJP.h"
#include "algorithmSDL.h"
#include "algorithmLDF.h"
#include "windows.h"
#include "psapi.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <array>


using namespace std;

/**
 * @parameters
 * 1) graph that will be used as input (number from 0 to graphPaths length )
 * 2) algorithm that will be run (
 *          0 = greedy
 *          1 = JP sequential
 *          2 = JP parallel
 *          3 = SDL sequential
 *          4 = SDL parallel
 *          5 = MIS sequential
 *          6 = MIS parallel
 * )
 */


int main(int argc, char ** argv) {
    int alg, nThreads = -1, maxColUsed = -1;
    string selectedAlg, selectedGraph, finalPath, basePath = "./graphs/benchmark/";
    vector<string> algorithms  = {"greedy", "JP sequential", "JP parallel","SDL sequential","SDL parallel", "MIS sequential", "MIS parallel", "LDF parallel"};
    vector<string> graphPaths = {/*0)*/"manual/v10.gra"/*1KB*/, "manual/v100.gra"/*13KB*/, "manual/v1000.gra"/*1.6MB*/,
            /*3)*/"small_sparse_real/agrocyc_dag_uniq.gra"/*1MB*/, "small_sparse_real/human_dag_uniq.gra"/*0.5MB*/, "small_dense_real/arXiv_sub_6000-1.gra"/*0.3MB*/, "scaleFree/ba10k5d.gra"/*0.2MB*/,
            // the next files are too large for git, need to import the "large" folder under "benchmark". It is already ignored in the .gitignore file
            /*7)*/"large/uniprotenc_150m.scc.gra"/*2MB*/, "large/citeseer.scc.gra"/*8MB*/, "large/uniprotenc_22m.scc.gra"/*19MB*/, "large/go_uniprot.gra"/*255MB*/,
            /*11)*/"large/citeseerx.gra" /*176MB*/, "large/cit-Patents.scc.gra" /*162MB*/, "large/uniprotenc_100m.scc.gra" /*232MB*/};

    //createBatchFile();// uncomment when required to create the batch file

    bool menuMode = false;
    vector<int> colors;
    map<int, int> graphNumberMap, nodesDegree;
    map<int, list<int>> randToNodesAssignedMap, graph;

    parametersSetup(selectedAlg, nThreads, menuMode, selectedGraph, finalPath, argc, argv, algorithms, graphPaths, basePath);

    graph = readGraph(finalPath, graphNumberMap, randToNodesAssignedMap, nodesDegree);

    do {

        if(!prerunSetup(colors, alg, menuMode, algorithms, nThreads, selectedGraph, argc, argv, graph))
            break;

        PROCESS_MEMORY_COUNTERS memCount;
        chrono::time_point<chrono::system_clock> startTime = chrono::system_clock::now();
        cout<<"Starting now"<<endl;

        switch(alg){

            case 0:{
                vector<int> colorsGreedy = greedyAssignment(graph, colors, &maxColUsed);

                //some output just to be sure the application ran properly
                cout << "number of nodes: " << graph.size() << endl;
                cout << "number of greedy colors: " << maxColUsed + 1 << endl;
                cout << "for instance color " << colorsGreedy[maxColUsed] << " was assigned to node " << maxColUsed << endl; //should never be -1

                break;
            }
            case 1:{
                vector<int> colorsJPS = jonesPlassmannSequentialAssignment(graph, graphNumberMap, colors, &maxColUsed);

                //some output just to be sure the application ran properly
                cout << "number of nodes: " << graph.size() << endl;
                cout << "number of JP colors: " << maxColUsed + 1 << endl;
                cout << "for instance color " << colorsJPS[maxColUsed] << " was assigned to node " << maxColUsed << endl; //should never be -1

                break;
            }
            case 2:{
                vector<int> colorsJP = jonesPlassmannParallelAssignment(graph, graphNumberMap, colors, &maxColUsed, nThreads);

                //some output just to be sure the application ran properly
                cout << "number of nodes: " << graph.size() << endl;
                cout << "number of JP colors: " << maxColUsed + 1 << endl;
                cout << "for instance color " << colorsJP[maxColUsed] << " was assigned to node " << maxColUsed << endl; //should never be -1

                break;
            }
            case 3:{
                vector<int> colorsSDLS = smallestDegreeLastSequentialAssignment(graph, colors, &maxColUsed);


                //some output just to be sure the application ran properly
                cout << "number of nodes: " << graph.size() << endl;
                cout << "number of SDLS colors: " << maxColUsed + 1 << endl;
                cout << "for instance color " << colorsSDLS[maxColUsed] << " was assigned to node " << maxColUsed << endl; //should never be -1
                break;

            }
            case 4:{
                vector<int> colorsSDLP = smallestDegreeLastParallelAssignment(graph, colors, &maxColUsed);

                //some output just to be sure the application ran properly
                cout << "number of nodes: " << graph.size() << endl;
                cout << "number of SDLP colors: " << maxColUsed + 1 << endl;
                cout << "for instance color " << colorsSDLP[maxColUsed] << " was assigned to node " << maxColUsed << endl; //should never be -1
                break;
            }

            case 5: {
                vector<int> colorsMIS = misSequentialAssignment(graph, colors, &maxColUsed);

                //some output just to be sure the application ran properly
                cout << "number of nodes: " << graph.size() << endl;
                cout << "number of MIS colors: " << maxColUsed + 1 << endl;
                cout << "for instance color " << colorsMIS[maxColUsed] << " was assigned to node " << maxColUsed << endl; //should never be -1
                break;
            }

            case 6: {
                vector<int> colorsMISP = misParallelAssignment(graph, graphNumberMap, colors, &maxColUsed, nThreads);

                //some output just to be sure the application ran properly
                cout << "number of nodes: " << graph.size() << endl;
                cout << "number of MIS colors: " << maxColUsed + 1 << endl;
                cout << "for instance color " << colorsMISP[maxColUsed] << " was assigned to node " << maxColUsed << endl; //should never be -1
                break;
            }

            case 7: {
                vector<int> colorsLDF = ldfParallelAssignment(graph, colors, nodesDegree, &maxColUsed, nThreads);

                //some output just to be sure the application ran properly
                cout << "number of nodes: " << graph.size() << endl;
                cout << "number of LDF colors: " << maxColUsed + 1 << endl;
                cout << "for instance color " << colorsLDF[maxColUsed] << " was assigned to node " << maxColUsed << endl; //should never be -1
                break;
            }

            default:{
                cout << "Selected non-existing algorithm";
                break;
            }
        }

        chrono::time_point<chrono::system_clock> endTime = chrono::system_clock::now();
        cout << "Time taken: " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;
        cout << endl;
        if (!menuMode) {
            GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&memCount, sizeof(memCount));
            cout << "Peak memory used: " << (double) memCount.PeakWorkingSetSize / 1024 / 1024 << " MB" << endl;
        }
        
    }while(menuMode == true);

    return 0;
}



