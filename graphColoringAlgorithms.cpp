#include "graphColoringAlgorithms.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <future>
#include <vector>

using namespace std;

mutex lpmutex, mismut;


//Starting with a sequential implementation of Jones-Plassman algorithm to check functionality and show a baseline
vector<int> jonesPlassmannSequentialAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed) {

    //Assign a random number to each vertex
    map<int, int> graphNumberMap = {};
    map<int,list<int>> nodeWeights={};

    for (auto const& node : graph) {
        int randNumber=rand()%(graph.size()*2);
        graphNumberMap[node.first] = randNumber;
        nodeWeights[randNumber].emplace_back(node.first);
    }

    map<int, list<int>> uncoloredNodes = graph;

    while (uncoloredNodes.size() > 0) {

        //Create independent set of vertices with the highest weights of all neighbours
     //   map<int, list<int>> independentSet{};
       // findIndependentSets(uncoloredNodes, graphNumberMap,graphNumberMap, independentSet);

        //In each independent set assign the minimum colour not belonging to a neighbour
        assignColoursSDL(uncoloredNodes, colors, graph, nodeWeights, maxColUsed);

    }

    return colors;
}

//Parallel implementation of Jones-Plassman algorithm
vector<int> jonesPlassmannParallelAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed) {

    //In the main thread assign a random number to each vertex
    map<int, int> graphNumberMap = {};

    /*for (auto const& node : graph) {
        graphNumberMap[node.first] = rand();
    }*/

    assignRandomNumbers(graph, graphNumberMap); // Thought I'd utilise the threads but doesn't appear much quicker

    map<int, list<int>> uncoloredNodes = graph;

    //Get maximum number of threads for the system
    int maxThreads = thread::hardware_concurrency();
    cout << "Max threads supported: " << maxThreads << endl;

    while (uncoloredNodes.size() > 0) {

        int nodesLeft = uncoloredNodes.size();

        if (nodesLeft >= maxThreads) {
            vector<thread> workers;
            //vector<map<int, list<int>>> nodesToColourVect;

            int stepSize = nodesLeft / maxThreads;

            //Lock uncolouredNodes while starting all the threads
            lpmutex.lock();

            for (int i = 0; i < maxThreads ; i++) {
                
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
                map<int, list<int>> nodesToColour(startIt, endIt);
                //nodesToColourVect.emplace_back(nodesToColour);

                //Create thread for set of uncoloured nodes
                workers.emplace_back([&uncoloredNodes, &colors, &graph, &graphNumberMap, maxColUsed, nodesToColour] {workerFunction(uncoloredNodes, colors, graph, graphNumberMap, maxColUsed, nodesToColour); });
                //workers.emplace_back([&uncoloredNodes, &colors, &graph, &graphNumberMap, maxColUsed, &nodesToColourVect, i] {workerFunction(uncoloredNodes, colors, graph, graphNumberMap, maxColUsed, nodesToColourVect, i); });

            }

            //Unlock uncolouredNodes so threads can update it
            lpmutex.unlock();

            //Wait for threads to finish before moving to next step
            for (auto& worker : workers) {
                    worker.join();
             }
        }
        else { //For last nodes where the number of nodes is less than the number of threads just use one thread
            //vector<map<int, list<int>>> nodesToColourVect;
            map<int, list<int>> nodesToColour(uncoloredNodes.begin(), uncoloredNodes.end());
            //nodesToColourVect.emplace_back(nodesToColour);
            thread worker([&uncoloredNodes, &colors, &graph, &graphNumberMap, maxColUsed, nodesToColour] {workerFunction(uncoloredNodes, colors, graph, graphNumberMap, maxColUsed, nodesToColour); });
            worker.join();
        }
        
    }

    return colors;
}

void workerFunction(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& graph, map<int, int>& graphNumberMap, int* maxColUsed, map<int, list<int>> nodesToColour) { //vector<map<int, list<int>>> &nodesToColourVect, int i ) { //
    //Create independent set of vertices with the highest weights of all neighbours
    map<int, list<int>> independentSet{};
    findIndependentSets(nodesToColour, graphNumberMap, graphNumberMap, independentSet);

    //In each independent set assign the minimum colour not belonging to a neighbour
    assignColours(uncoloredNodes, colors, graph, independentSet, maxColUsed);
}

void assignRandomNumbers(map<int, list<int>>& graph, map<int, int>& graphNumberMap) {

    int maxThreads = thread::hardware_concurrency();

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

vector<int> smallestDegreeLastSequentialAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed){
    int i=1;
    int k=0;
    map<int,list<int>> unweightedGraph(graph);
    map<int,list<int>> tempGraph(graph);
    map<int,list<int>> nodeWeights={};
    map<int, int> graphNumberMap = {};
    map<int, int> graphNumberRandMap = {};
    chrono::time_point<chrono::system_clock> startTime = chrono::system_clock::now();
    while(!unweightedGraph.empty()){
        for(auto const& node : unweightedGraph ){
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
        assignColoursSDL(uncoloredNodes, colors, graph, nodeWeights, maxColUsed);

    }

    return colors;


}
void assignColoursSDL(map<int, list<int>> &uncoloredNodes, vector<int> &colors, map<int, list<int>> &graph, map<int, list<int>> &nodeWeights, int* maxColUsed) {
    //In each independent set assign the minimum colour not belonging to a neighbour
    for (int i=nodeWeights.size();i>0;i--) {
        for(const auto node: nodeWeights[i]){
            //Build set of neighbours colours
            vector<int> neighborColors = {};
            for (auto& neighbor : graph.at(node)) {
                if (colors[neighbor] != -1)
                    neighborColors.push_back(colors[neighbor]);
            }

            //assign lowest color not in use by any neighbor
            int selectedCol = 0;
            while (colors[node] == -1) {
                if (find(neighborColors.begin(), neighborColors.end(), selectedCol) != neighborColors.end()) {
                    /* neighborColors contains selectedCol */
                    selectedCol++;
                }
                else {
                    colors[node] = selectedCol;
                    if (selectedCol > *maxColUsed)
                        *maxColUsed = selectedCol;
                }
            }

        //Remove coloured nodes from uncolouredNodes, and from neighbours in uncolouredNodes
        //Encased in a mutex to ensure atomicity
        lpmutex.lock();
        for (auto& neighbor : uncoloredNodes[node]) {
            uncoloredNodes[neighbor].remove(node);
        }
        uncoloredNodes.erase(node);
        lpmutex.unlock();
    }
    }
}
vector<int> smallestDegreeLastParallelAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed){
    vector<thread> threads;
    int i=1;
    int k=0;
    map<int,list<int>> unweightedGraph(graph);
    map<int,list<int>> tempGraph(graph);
    map<int,list<int>> nodeWeights={};
    map<int, int> graphNumberMap = {};
    map<int, int> graphNumberRandMap = {};
    chrono::time_point<chrono::system_clock> startTime = chrono::system_clock::now();

    while(!unweightedGraph.empty()){
        lpmutex.lock();
        for(auto const& node : unweightedGraph ){
            if(node.second.size()<=k){
                threads.emplace_back([node,&graphNumberMap,i,&unweightedGraph,&nodeWeights,&graphNumberRandMap,&graph,&tempGraph] {parallelWeighting(node,graphNumberMap,i,unweightedGraph,nodeWeights,graphNumberRandMap,graph,tempGraph);});
            }
        }
        lpmutex.unlock();
        for(auto& thread : threads){
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
        assignColoursSDL(uncoloredNodes, colors, graph, nodeWeights, maxColUsed);
    }

    return colors;
}


void parallelWeighting(pair<int, list<int>> node,map<int, int> &graphNumberMap, int i, map<int,list<int>> &unweightedGraph, map<int,list<int>> &nodeWeights, map<int, int> &graphNumberRandMap,  map<int,list<int>> &graph, map<int,list<int>> &tempGraph){
    graphNumberMap[node.first]=i;
    nodeWeights[i].emplace_back(node.first);
    graphNumberRandMap[node.first]=rand()%(graph.size()*2);
    for(const auto neighbour : node.second){
        tempGraph[neighbour].remove(node.first);
    }
    tempGraph.erase(node.first);
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
vector<int> misParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed) {

    int colour = 0;

    //In the main thread assign a random number to each vertex
    map<int, int> graphNumberMap = {};
    assignRandomNumbers(graph, graphNumberMap); 

    map<int, list<int>> uncoloredNodes = graph;

    //Get maximum number of threads for the system
    int maxThreads = thread::hardware_concurrency();
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
        assignColoursParallel(uncoloredNodes, colors, maxIndependentSet, colour);

        // Go to the next colour
        colour++;
    }

    *maxColUsed = colour;

    return colors;
}

void assignColoursParallel(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour) {

    int maxThreads = thread::hardware_concurrency();

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


