#include "graphColoringAlgorithms.h"
#include <thread>
#include <iostream>
#include <mutex>

using namespace std;

mutex lpmutex;

vector<int> greedyAssignment(map<int, list<int>> graph, vector<int> colors, int *maxColUsed) {

    vector<int> neighborColors;
    int selectedCol;

    for (auto const& node : graph)
    {
        neighborColors = {};
        selectedCol = 0;

        //find all neighbors' colours
        for (auto & neighbor : node.second) {
            if (colors[neighbor] != -1)
                neighborColors.push_back(colors[neighbor]);
        }

        //assign lowest color not in use by any neighbor
        while(colors[node.first]== -1){
            if(find(neighborColors.begin(), neighborColors.end(), selectedCol) != neighborColors.end()) {
                /* neighborColors contains selectedCol */
                selectedCol++;
            }else{
                colors[node.first] = selectedCol;
                if(selectedCol > *maxColUsed)
                    *maxColUsed = selectedCol;
            }

        }
    }

    return colors;

}

//Starting with a sequential implementation of Jones-Plassman algorithm to check functionality and show a baseline
vector<int> jonesPlassmannSequentialAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed) {

    //Assign a random number to each vertex
    map<int, int> graphNumberMap = {};

    for (auto const& node : graph) {
        graphNumberMap[node.first] = rand();
    }

    map<int, list<int>> uncoloredNodes = graph;

    while (uncoloredNodes.size() > 0) {

        //Create independent set of vertices with the highest weights of all neighbours
        map<int, list<int>> independentSet = findIndependentSets(uncoloredNodes, graphNumberMap);

        //In each independent set assign the minimum colour not belonging to a neighbour
        assignColours(uncoloredNodes, colors, graph, independentSet, maxColUsed);

    }

    return colors;
}

//Parallel implementation of Jones-Plassman algorithm
vector<int> jonesPlassmannParallelAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed) {

    //In the main thread assign a random number to each vertex
    map<int, int> graphNumberMap = {};

    for (auto const& node : graph) {
        graphNumberMap[node.first] = rand();
    }

    map<int, list<int>> uncoloredNodes = graph;

    //Get maximum number of threads for the system
    int maxThreads = thread::hardware_concurrency();
    cout << "Max threads supported: " << maxThreads << endl;

    while (uncoloredNodes.size() > 0) {

        int nodesLeft = uncoloredNodes.size();

        if (nodesLeft >= maxThreads) {
            vector<thread> workers;

            for (int i = 0; i < maxThreads ; i++) {
                
                //Split the remaining nodes by the number of threads
                map<int, list<int>>::iterator startIt = uncoloredNodes.begin();
                advance(startIt, i * nodesLeft / maxThreads);
                map<int, list<int>>::iterator endIt;
                if (i = maxThreads - 1) {
                    endIt = uncoloredNodes.end(); // Bound for the end nodes
                }
                else {
                    endIt = uncoloredNodes.begin();
                    advance(endIt, i * nodesLeft / maxThreads + nodesLeft / maxThreads); 
                }
                map<int, list<int>> nodesToColour(startIt, endIt);

                //Create thread for set of uncoloured nodes
                workers.emplace_back([&uncoloredNodes, &colors, &graph, &graphNumberMap, maxColUsed, nodesToColour] {workerFunction(uncoloredNodes, colors, graph, graphNumberMap, maxColUsed, nodesToColour); });
                
            }

            //Wait for threads to finish before moving to next step
            for (auto& worker : workers) {
                    worker.join();
             }
        }
        else { //For last nodes where the number of nodes is less than the number of threads just use one thread
            map<int, list<int>> nodesToColour(uncoloredNodes.begin(), uncoloredNodes.end());
            thread worker([&uncoloredNodes, &colors, &graph, &graphNumberMap, maxColUsed, nodesToColour] {workerFunction(uncoloredNodes, colors, graph, graphNumberMap, maxColUsed, nodesToColour); });
            worker.join();
        }
        
    }

    return colors;
}

void workerFunction(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& graph, map<int, int>& graphNumberMap, int* maxColUsed, map<int, list<int>> nodesToColour) {
    //Create independent set of vertices with the highest weights of all neighbours
    map<int, list<int>> independentSet = findIndependentSets(nodesToColour, graphNumberMap);

    //In each independent set assign the minimum colour not belonging to a neighbour
    assignColours(uncoloredNodes, colors, graph, independentSet, maxColUsed);
}

map<int, list<int>> findIndependentSets(map<int, list<int>> myUncoloredNodes, map<int, int> &graphNumberMap) {
    //Create independent set of vertices with the highest weights of all neighbours
    map<int, list<int>> independentSet{};
    for (auto const& node : myUncoloredNodes) {
        bool maxNode = true;
        for (auto const& neighbour : node.second) {
            if (graphNumberMap[node.first] < graphNumberMap[neighbour]) {
                maxNode = false;
            }
            else if (graphNumberMap[node.first] == graphNumberMap[neighbour]) {
                int node1= rand();
                int neighbour1= rand();
                if (node1 < neighbour1) {
                    maxNode = false;
                }
            }
        }
        if (maxNode) {
            independentSet.emplace(node);
        }
    }
    return independentSet;
}

void assignColours(map<int, list<int>> &uncoloredNodes, vector<int> &colors, map<int, list<int>> &graph, map<int, list<int>> independentSet, int* maxColUsed) {
    //In each independent set assign the minimum colour not belonging to a neighbour
    for (auto const& node : independentSet) {

        //Build set of neighbours colours
        vector<int> neighborColors = {};
        for (auto& neighbor : graph.at(node.first)) {
            if (colors[neighbor] != -1)
                neighborColors.push_back(colors[neighbor]);
        }

        //assign lowest color not in use by any neighbor
        int selectedCol = 0;
        while (colors[node.first] == -1) {
            if (find(neighborColors.begin(), neighborColors.end(), selectedCol) != neighborColors.end()) {
                /* neighborColors contains selectedCol */
                selectedCol++;
            }
            else {
                colors[node.first] = selectedCol;
                if (selectedCol > *maxColUsed)
                    *maxColUsed = selectedCol;
            }
        }

        //Remove coloured nodes from uncolouredNodes, and from neighbours in uncolouredNodes
        //Encased in a mutex to ensure atomicity
        lpmutex.lock(); 
        uncoloredNodes.erase(node.first);
        for (auto& neighbor : node.second) {
            uncoloredNodes[neighbor].remove(node.first);
        }
        lpmutex.unlock();
    }



}
vector<int> smallestDegreeLastSequentialAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed){
    int i=1;
    int minDegree=graph.size();
    for(const auto node:graph){
        if(node.second.size()<minDegree)
            minDegree=node.second.size();
    }
    int k=minDegree;
    map<int,list<int>> unweightedGraph(graph);
    map<int,list<int>> tempGraph(graph);
    vector<int> weightedNodes={};
    //Assign a random number to each vertex
    map<int, int> graphNumberMap = {};
    while(!unweightedGraph.empty()){
        int actualSize=tempGraph.size();
        for(auto const& node : unweightedGraph ){
            if(node.second.size()<=k){
                graphNumberMap[node.first]=i;
                for(const auto neighbour : node.second){
                    tempGraph[neighbour].remove(node.first);
                }
                tempGraph.erase(node.first);
            }
        }
        unweightedGraph.operator=(tempGraph);
        if(tempGraph.size()<actualSize)
             i++;
        k++;
    }

    map<int, list<int>> uncoloredNodes = graph;

    while (uncoloredNodes.size() > 0) {

        //Create independent set of vertices with the highest weights of all neighbours
        map<int, list<int>> independentSet = findIndependentSets(uncoloredNodes, graphNumberMap);

        //In each independent set assign the minimum colour not belonging to a neighbour
        assignColours(uncoloredNodes, colors, graph, independentSet, maxColUsed);

    }

    return colors;


}

vector<int> smallestDegreeLastParallelAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed){


    map<int,int> graphNumberMap = weightNodes(graph);

    map<int, list<int>> uncoloredNodes = graph;

    while (uncoloredNodes.size() > 0) {

        //Create independent set of vertices with the highest weights of all neighbours
        map<int, list<int>> independentSet = findIndependentSets(uncoloredNodes, graphNumberMap);

        //In each independent set assign the minimum colour not belonging to a neighbour
        assignColours(uncoloredNodes, colors, graph, independentSet, maxColUsed);

    }

    return colors;
}

map<int,int> weightNodes(map<int,list<int>> graph){
    int i=1;
    int k=1;
    vector<thread> threads;
    map<int, int> graphNumberMap = {};
    map<int,list<int>> unweightedGraph(graph);
    while(!unweightedGraph.empty()){
        for(const auto node : unweightedGraph ){
            if(node.second.size()<=k)
                threads.emplace_back([node,&graphNumberMap,i,&unweightedGraph] {parallelWeighting(node,graphNumberMap,i,unweightedGraph);});
        }
        i++;
        k++;
    }
    return graphNumberMap;
}

void parallelWeighting(pair<int, list<int>> node,map<int, int> graphNumberMap, int i, map<int,list<int>> unweightedGraph){
    graphNumberMap[node.first]=i;
    for(const auto neighbour : node.second){
        unweightedGraph[neighbour].remove(node.first);
    }
    unweightedGraph.erase(node.first);
}