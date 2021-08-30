//
// Created by Marco Pappalardo on 30/08/2021.
//
#include "algorithmSDL.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <future>
#include <vector>

using namespace std;

mutex sdlmutex;
int semSDLColor, semSDLNodes;


vector<int> smallestDegreeLastSequentialAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed){
    int i=1;
    int k=0;
    map<int,list<int>> unweightedGraph(graph);
    map<int,list<int>> nodeWeights={};
    map<int, int> graphNumberRandMap = {};
    vector<int> toBeRemoved={};
    chrono::time_point<chrono::system_clock> startTime = chrono::system_clock::now();
    while(!unweightedGraph.empty()){
        for(auto const& node : unweightedGraph ){
            if(node.second.size()<=k){
                nodeWeights[i].emplace_back(node.first);
                graphNumberRandMap[node.first]=rand()%(graph.size()*2);
                for(const auto neighbour : node.second){
                    unweightedGraph[neighbour].remove(node.first);
                }
                toBeRemoved.emplace_back(node.first);
            }
        }
        if(toBeRemoved.size()>0){
            i++;
            for(auto node : toBeRemoved){
                unweightedGraph.erase(node);
            }
            toBeRemoved.clear();
        }

        k++;
    }
    chrono::time_point<chrono::system_clock> endTime = chrono::system_clock::now();
    cout << "Time taken for weighting: " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;

    map<int, list<int>> uncoloredNodes = graph;

    while (uncoloredNodes.size() > 0) {

        //Create independent set of vertices with the highest weights of all neighbours
        // map<int, list<int>> independentSet{};
        //       findIndependentSets(uncoloredNodes, graphNumberMap, graphNumberRandMap, independentSet);

        //In each independent set assign the minimum colour not belonging to a neighbour
        assignColoursSDL(uncoloredNodes, colors,  nodeWeights, maxColUsed);

    }

    return colors;


}
void assignColoursSDL(map<int, list<int>> &uncoloredNodes, vector<int> &colors, map<int, list<int>> &nodeWeights, int* maxColUsed) {
    //In each independent set assign the minimum colour not belonging to a neighbour
    map<int,list<int>> neighborColors = {};
    for (int i=nodeWeights.size();i>0;i--) {
        for(const auto node: nodeWeights[i]){
            neighborColors[node].sort();

            //Build set of neighbours colours

            //assign lowest color not in use by any neighbor
            int selectedCol = 0;
            int previous=0;
            for(auto neighborCol : neighborColors[node])
            {
                if(neighborCol-previous>1){
                    selectedCol=previous+1;
                    break;
                }else{
                    selectedCol++;
                }
                previous=neighborCol;
            }
            colors[node] = selectedCol;
            if (selectedCol > *maxColUsed)
                *maxColUsed = selectedCol;


            //Remove coloured nodes from uncolouredNodes, and from neighbours in uncolouredNodes
            //Encased in a mutex to ensure atomicity
            sdlmutex.lock();
            for (auto& neighbor : uncoloredNodes[node]) {
                uncoloredNodes[neighbor].remove(node);
                neighborColors[neighbor].emplace_back(selectedCol);
            }
            uncoloredNodes.erase(node);
            sdlmutex.unlock();

        }
    }

}
vector<int> smallestDegreeLastParallelAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed){
    vector<thread> workers;
    int maxThreads = thread::hardware_concurrency();
    cout << "Max threads supported: " << maxThreads << endl;
    semSDLColor = maxThreads;
    semSDLNodes = maxThreads;



    int i=1;
    int k=0;
    map<int,list<int>> unweightedGraph(graph);
    map<int,list<int>> tempGraph(graph);
    map<int,list<int>> nodeWeights={};
    map<int, int> graphNumberMap = {};
    map<int, int> graphNumberRandMap = {};
    chrono::time_point<chrono::system_clock> startTime = chrono::system_clock::now();

    while(!unweightedGraph.empty()){
        sdlmutex.lock();
        for(int i=0;i<maxThreads;i++){

                workers.emplace_back([node,&graphNumberMap,i,&unweightedGraph,&nodeWeights,&graphNumberRandMap,&graph,&tempGraph] {parallelWeighting(node,graphNumberMap,i,unweightedGraph,nodeWeights,graphNumberRandMap,graph,tempGraph);});

        }
        sdlmutex.unlock();
        for(auto& thread : workers){
            thread.join();
        }
        if(unweightedGraph.size()>tempGraph.size()){
            i++;
            unweightedGraph.operator=(tempGraph);
        }

        k++;
    }
    chrono::time_point<chrono::system_clock> endTime = chrono::system_clock::now();
    cout << "Time taken for weighting: " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;

    map<int, list<int>> uncoloredNodes = graph;
    while (uncoloredNodes.size() > 0) {

        //Create independent set of vertices with the highest weights of all neighbours
        // map<int, list<int>> independentSet{};
        //       findIndependentSets(uncoloredNodes, graphNumberMap, graphNumberRandMap, independentSet);

        //In each independent set assign the minimum colour not belonging to a neighbour
        assignColoursSDL(uncoloredNodes, colors,  nodeWeights, maxColUsed);
    }

    return colors;
}


void parallelWeighting(pair<int, list<int>> node,map<int, int> &graphNumberMap, int i, map<int,list<int>> &unweightedGraph, map<int,list<int>> &nodeWeights, map<int, int> &graphNumberRandMap,  map<int,list<int>> &graph, map<int,list<int>> &tempGraph){
    if(node.second.size()<=k){
    graphNumberMap[node.first]=i;
    nodeWeights[i].emplace_back(node.first);
    graphNumberRandMap[node.first]=rand()%(graph.size()*2);

        for(const auto neighbour : node.second){
            tempGraph[neighbour].remove(node.first);
        }
        tempGraph.erase(node.first);
    }
}