#include "graphColoringAlgorithms.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <future>
#include <vector>

using namespace std;

mutex lpmutex, mismut;


void workerFunction(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& graph, map<int, int>& graphNumberMap, int* maxColUsed, map<int, list<int>> nodesToColour) { //vector<map<int, list<int>>> &nodesToColourVect, int i ) { //
    //Create independent set of vertices with the highest weights of all neighbours
    map<int, list<int>> independentSet{};
    findIndependentSets(nodesToColour, graphNumberMap, graphNumberMap, independentSet);

    //In each independent set assign the minimum colour not belonging to a neighbour
    assignColours(uncoloredNodes, colors, graph, independentSet, maxColUsed);
}

void assignRandomNumbers(map<int, list<int>>& graph, map<int, int>& graphNumberMap, int nThreads) {

    int maxThreads = (nThreads <= 0 || nThreads > thread::hardware_concurrency()) ? thread::hardware_concurrency() : nThreads ;

    vector<thread> workers;

    int stepSize = graph.size() / maxThreads;

    for (int i = 0; i < maxThreads; i++) {

        //Split the remaining nodes by the number of threads
        map<int, list<int>>::iterator startIt = graph.begin();
        advance(startIt, i * stepSize);
        map<int, list<int>>::iterator endIt;
        if (i == maxThreads - 1) {
            endIt = graph.end(); // Bound for the end nodes
        }
        else {
            endIt = graph.begin();
            advance(endIt, i * stepSize + stepSize);
        }
        map<int, list<int>> nodesToColour(startIt, endIt);

        //Create thread for set of uncoloured nodes
        workers.emplace_back([&graph, &graphNumberMap] {assignNumberWorker(graph, graphNumberMap); });

    }

    //Wait for threads to finish before moving to next step
    for (auto& worker : workers) {
        worker.join();
    }
}

void assignNumberWorker(map<int, list<int>>& graph, map<int, int>& graphNumberMap) {
    for (auto const& node : graph) {
        graphNumberMap[node.first] = rand();
    }
}

void findIndependentSets(map<int, list<int>> &myUncoloredNodes, map<int, int> &graphNumberMap,map<int, int> &graphNumberRandMap, map<int, list<int>> &independentSet) {
    //Create independent set of vertices with the highest weights of all neighbours
    // map<int, list<int>> independentSet{};
    for(const auto  & node: myUncoloredNodes){
        findMaxNode(node,myUncoloredNodes,graphNumberMap, graphNumberRandMap ,independentSet);
    }
    // return independentSet;
}

void findIndependentSetsParallel(map<int, list<int>> &myUncoloredNodes, map<int, int> &graphNumberMap, map<int, list<int>> &independentSet) {
    //Create independent set of vertices with the highest weights of all neighbours
   // map<int, list<int>> independentSet{};
   vector<thread> threads;
    for(const auto  & node: myUncoloredNodes){
    //    threads.emplace_back([&node,&myUncoloredNodes, &graphNumberMap,&independentSet] { findMaxNode(node,myUncoloredNodes,graphNumberMap,independentSet); });
    }


   // return independentSet;
}


void findMaxNode(pair<int,list<int>> node,map<int, list<int>> &myUncoloredNodes,map<int, int> &graphNumberMap,map<int, int> &graphNumberRandMap, map<int, list<int>> &independentSet){
    int max=0;
    int maxRand=0;
    for(auto neighbor: node.second){
        if(graphNumberMap[neighbor]>max){
            max=graphNumberMap[neighbor];
            maxRand=0;

        }else if(graphNumberMap[neighbor]==max && graphNumberRandMap[neighbor]>maxRand){
            maxRand=graphNumberRandMap[neighbor];
        }
    }
    if(max<graphNumberMap[node.first]){
        independentSet.emplace(node);
    }
    else if(max==graphNumberMap[node.first]){
        if(maxRand<=graphNumberRandMap[node.first])
          independentSet.emplace(node);
    }
    /*if(isMax)
        independentSet.emplace(node);*/
}

void assignColours(map<int, list<int>> &uncoloredNodes, vector<int> &colors, map<int, list<int>> &graph, map<int, list<int>> &independentSet, int* maxColUsed) {
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
void findIndependentSetsMIS(map<int, list<int>> myUncoloredNodes, map<int, int>& graphNumberMap, map<int, list<int>> &maxIndependentSet) {
    //Create independent set of vertices with the highest weights of all neighbours
   // map<int, list<int>> independentSet{};
    for (auto const& node : myUncoloredNodes) {
        bool maxNode = true;
        for (auto const& neighbour : node.second) {
            if (graphNumberMap[node.first] < graphNumberMap[neighbour]) {
                maxNode = false;
                break;
            }
            else if (graphNumberMap[node.first] == graphNumberMap[neighbour]) {
                int node1 = rand();
                int neighbour1 = rand();
                if (node1 < neighbour1) {
                    maxNode = false;
                    break;
                }
            }
        }
        if (maxNode) {
            //independentSet.emplace(node);
            // Add to max set
            mismut.lock();
            maxIndependentSet.emplace(node);
            mismut.unlock();
        }
    }
    //return independentSet;
}

//Parallel implementation of Maximal Independent Set algorithm to check functionality and show a baseline
vector<int> misParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed, int nThreads) {

    int colour = 0;

    //In the main thread assign a random number to each vertex
    map<int, int> graphNumberMap = {};
    assignRandomNumbers(graph, graphNumberMap, nThreads);

    map<int, list<int>> uncoloredNodes = graph;

    //Get maximum number of threads for the system
    int maxThreads = (nThreads <= 0 || nThreads > thread::hardware_concurrency()) ? thread::hardware_concurrency() : nThreads ;
    cout << "Max threads supported: " << maxThreads << endl;

    while (uncoloredNodes.size() > 0) {

        int nodesLeft = uncoloredNodes.size();

        // Calculate maximal independent set
        map<int, list<int>> maxIndependentSet{};

        if (nodesLeft >= maxThreads) {
            vector<thread> workers;

            int stepSize = nodesLeft / maxThreads;

            for (int i = 0; i < maxThreads; i++) {

                //Split the remaining nodes by the number of threads
                map<int, list<int>>::iterator startIt = uncoloredNodes.begin();
                advance(startIt, i * stepSize);
                map<int, list<int>>::iterator endIt;
                if (i == maxThreads - 1) {
                    endIt = uncoloredNodes.end(); // Bound for the end nodes
                }
                else {
                    endIt = uncoloredNodes.begin();
                    advance(endIt, i * stepSize + stepSize);
                }
                map<int, list<int>> nodesToConsider(startIt, endIt);

                //Create thread for set of uncoloured nodes
                workers.emplace_back([nodesToConsider, &graphNumberMap, &maxIndependentSet] {findIndependentSetsMIS(nodesToConsider, graphNumberMap, maxIndependentSet); });

            }

            //Add independent sets together to form maximal set
            for (auto& worker : workers) {
                worker.join();
            }

        }
        else { //For last nodes where the number of nodes is less than the number of threads just use one thread
            map<int, list<int>> nodesToConsider(uncoloredNodes.begin(), uncoloredNodes.end());
            thread worker([nodesToConsider, &graphNumberMap, &maxIndependentSet] {findIndependentSetsMIS(nodesToConsider, graphNumberMap, maxIndependentSet); });
            worker.join();
        }

        //In each independent set assign the current colour 
        //assignColoursMIS(uncoloredNodes, colors, graph, maxIndependentSet, colour); // Sequential
        assignColoursParallel(uncoloredNodes, colors, maxIndependentSet, colour, nThreads);

        // Go to the next colour
        colour++;
    }

    *maxColUsed = colour;

    return colors;
}

void assignColoursParallel(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour, int nThreads) {

    int maxThreads = (nThreads <= 0 || nThreads > thread::hardware_concurrency()) ? thread::hardware_concurrency() : nThreads ;

    vector<thread> workers;

    int stepSize = maximalIndependentSet.size() / maxThreads;

    for (int i = 0; i < maxThreads; i++) {

        //Split the remaining nodes by the number of threads
        map<int, list<int>>::iterator startIt = maximalIndependentSet.begin();
        advance(startIt, i * stepSize);
        map<int, list<int>>::iterator endIt;
        if (i == maxThreads - 1) {
            endIt = maximalIndependentSet.end(); // Bound for the end nodes
        }
        else {
            endIt = maximalIndependentSet.begin();
            advance(endIt, i * stepSize + stepSize);
        }
        map<int, list<int>> nodesToColour(startIt, endIt);

        //Create thread for set of uncoloured nodes
        workers.emplace_back([&uncoloredNodes, &colors, nodesToColour, colour] {assignColoursWorker(uncoloredNodes, colors, nodesToColour, colour); });

    }

    //Wait for threads to finish before moving to next step
    for (auto& worker : workers) {
        worker.join();
    }
}


void assignColoursWorker(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>> maximalIndependentSet, int colour) {

    // Assign colour to nodes in the maximal independent set
    for (auto const& node : maximalIndependentSet) {
        colors[node.first] = colour;

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


