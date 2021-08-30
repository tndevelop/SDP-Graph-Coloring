#include "graphColoringAlgorithms.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <future>
#include <vector>

using namespace std;

mutex lpmutex, mismutex;

vector<int> greedyAssignment(map<int, list<int>> graph, vector<int> colors, int *maxColUsed) {
    vector<int> neighborColors;
    int selectedCol;
    int percentage = graph.size() / 100;
    int count = 0;

    for (auto const& node : graph)
    {
        /*
        count++;
        if (percentage != 0 && count % percentage == 0)
            cout << count / percentage << "%" << endl;
        */
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
    map<int,list<int>> nodeWeights={};

    for (auto const& node : graph) {
        int randNumber=rand()%(graph.size()/5);
        graphNumberMap[node.first] = randNumber;
        nodeWeights[randNumber].emplace_back(node.first);
    }

    map<int, list<int>> uncoloredNodes = graph;

    while (uncoloredNodes.size() > 0) {

        //Create independent set of vertices with the highest weights of all neighbours
        map<int, list<int>> independentSet{};
       // findIndependentSets(uncoloredNodes, graphNumberMap,graphNumberMap, independentSet);

        //In each independent set assign the minimum colour not belonging to a neighbour
        assignColoursSDL(uncoloredNodes, colors, nodeWeights, maxColUsed);

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
        lpmutex.lock();
        for (auto& neighbor : uncoloredNodes[node]) {
            uncoloredNodes[neighbor].remove(node);
            neighborColors[neighbor].emplace_back(selectedCol);
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
        assignColoursSDL(uncoloredNodes, colors,  nodeWeights, maxColUsed);
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

void findMaximalIndependentSet(map<int, list<int>>& myUncoloredNodes, map<int, list<int>>& maximalIndependentSet) {
    for (auto const& node : myUncoloredNodes) {
        bool maxNode = true;
        for (auto const& neighbour : node.second) {
            if (maximalIndependentSet.count(neighbour) > 0) {
                maxNode = false;
                break;
            }
        }
        if (maxNode) {
            maximalIndependentSet.emplace(node);
        }
    }
}

void assignColoursMIS(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour) {
    
    // Assign colour to nodes in the maximal independent set
    for (auto const& node : maximalIndependentSet) {
        colors[node.first] = colour;  

        //Remove coloured nodes from uncolouredNodes, and from neighbours in uncolouredNodes
        //Encased in a mutex to ensure atomicity
        mismutex.lock();
        uncoloredNodes.erase(node.first);
        for (auto& neighbor : node.second) {
            uncoloredNodes[neighbor].remove(node.first);
        }
        mismutex.unlock();
    }

}

//Sequential implementation of Maximal Independent Set algorithm to check functionality and show a baseline
vector<int> misSequentialAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed) {

    int colour = 0;

    map<int, list<int>> uncoloredNodes = graph;

    while (uncoloredNodes.size() > 0) {

        //Create maximal independent set of vertices 
        map<int, list<int>> maxIndependentSet{};
        findMaximalIndependentSet(uncoloredNodes, maxIndependentSet);

        //In each independent set assign the current colour 
     
        assignColoursMIS(uncoloredNodes, colors, maxIndependentSet, colour);
        // Go to the next colour
        colour++;
    }

    *maxColUsed = colour;

    return colors;
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
            mismutex.lock();
            maxIndependentSet.emplace(node);
            mismutex.unlock();
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

//// Implementation of Luby's version of the algorithm (in progress)
//vector<int> lubyMISParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed) {
//
//    int colour = 0;
//
//    map<int, list<int>> uncoloredNodes = graph;
//
//    //Get maximum number of threads for the system
//    int maxThreads = thread::hardware_concurrency();
//    cout << "Max threads supported: " << maxThreads << endl;
//
//    while (uncoloredNodes.size() > 0) {
//
//        int nodesLeft = uncoloredNodes.size();
//
//        // Calculate maximal independent set
//        map<int, list<int>> maxIndependentSet{};
//        lubysAlgorithm();
//        
//
//        //In each independent set assign the current colour 
//        assignColoursParallel(uncoloredNodes, colors, maxIndependentSet, colour);
//
//        // Go to the next colour
//        colour++;
//    }
//
//    *maxColUsed = colour;
//
//    return colors;
//}
//
//void lubysAlgorithm(map<int, list<int>>& nodes, map<int, list<int>>& I, int* numThreads) {
//
//    map<int, list<int>> ghat = nodes;
//
//    while (ghat.size() > 0) {
//
//        map<int, list<int>> X{};
//
//
//
//    }
//
//}
//
//void firstStepLuby(map<int, list<int>>& ghat, map<int, list<int>> &X, int iterStep, int startOffset) {
//
//    // Step through the nodes with the step size of number of threads
//    map<int, list<int>>::iterator index = ghat.begin();
//    advance(index, startOffset);
//
//    map<int, list<int>>::iterator end = ghat.end();
//    prev(end, iterStep - startOffset);
//
//    while (index != ghat.end()) {
//
//    }
//
//}

//Parallel implementation of Maximal Independent Set algorithm to check functionality and show a baseline
vector<int> misIteratorsParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed) {

    int colour = 0;

    ////Assign a random number to each vertex
    //map<int, int> graphNumberMap = {};
    //map<int, list<int>> nodeWeights = {};

    //for (auto const& node : graph) {
    //    int randNumber = rand() % (graph.size() * 2);
    //    graphNumberMap[node.first] = randNumber;
    //    nodeWeights[randNumber].emplace_back(node.first);
    //}

    //In the main thread assign a random number to each vertex
    map<int, int> graphNumberMap = {};
    assignRandomNumbersIters(graph, graphNumberMap);

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

                //Create thread for set of uncoloured nodes
                workers.emplace_back([&uncoloredNodes, &graphNumberMap, &maxIndependentSet, i, maxThreads] {findIndependentSetsMISIters(uncoloredNodes, graphNumberMap, maxIndependentSet, i, maxThreads); });

            }

            //Add independent sets together to form maximal set
            for (auto& worker : workers) {
                worker.join();
            }

        }
        else { //For last nodes where the number of nodes is less than the number of threads just use one thread
            thread worker([&uncoloredNodes, &graphNumberMap, &maxIndependentSet] {findIndependentSetsMISIters(uncoloredNodes, graphNumberMap, maxIndependentSet, 0, 1); });
            worker.join();
        }

        //In each independent set assign the current colour 
        assignColoursParallelIters(uncoloredNodes, colors, maxIndependentSet, colour);

        // Go to the next colour
        colour++;
    }

    *maxColUsed = colour;

    return colors;
}


void findIndependentSetsMISIters(map<int, list<int>>& uncoloredNodes, map<int, int>& graphNumberMap, map<int, list<int>>& maxIndependentSet, int startOffset, int stepSize) {
    //Create independent set of vertices with the highest weights of all neighbours
    map<int, list<int>> independentSet{};
    
    // Step through the nodes with the step size of number of threads
    map<int, list<int>>::iterator iter = uncoloredNodes.begin();
    advance(iter, startOffset);

    int i = startOffset;
    do {
        bool maxNode = true;
        for (auto const& neighbour : iter->second) {
            if (graphNumberMap[iter->first] < graphNumberMap[neighbour]) {
                maxNode = false;
                break;
            }
            else if (graphNumberMap[iter->first] == graphNumberMap[neighbour]) {
                if (iter->first < neighbour) {
                    maxNode = false;
                    break;
                }
            }
        }

        if (maxNode) {
            independentSet.emplace(*iter);
        }

        i += stepSize;
        //Go to next node of thread
        if (i >= uncoloredNodes.size()) {
            break;
        }
        advance(iter, stepSize);
        
    } while (i < uncoloredNodes.size());

    mismutex.lock();
    for (auto const& node : independentSet) {
        // Add to max set
        maxIndependentSet.emplace(node);
    }
    mismutex.unlock();
}

void assignColoursParallelIters(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour) {

    int maxThreads = thread::hardware_concurrency();

    vector<thread> workers;

    int stepSize = maximalIndependentSet.size() / maxThreads;

    if (maximalIndependentSet.size() >= maxThreads) {
        for (int i = 0; i < maxThreads; i++) {

            //Create thread for set of uncoloured nodes
            workers.emplace_back([&uncoloredNodes, &colors, &maximalIndependentSet, colour, i, maxThreads] {assignColoursWorkerIters(uncoloredNodes, colors, maximalIndependentSet, colour, i, maxThreads); });

        }

        //Wait for threads to finish before moving to next step
        for (auto& worker : workers) {
            worker.join();
        }
    }
    else {
        thread worker([&uncoloredNodes, &colors, &maximalIndependentSet, colour]{assignColoursWorkerIters(uncoloredNodes, colors, maximalIndependentSet, colour, 0, 1); });
        worker.join();
    }
}


void assignColoursWorkerIters(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour, int startOffset, int stepSize) {

    // Step through the nodes with the step size of number of threads
    map<int, list<int>>::iterator iter = maximalIndependentSet.begin();
    advance(iter, startOffset);

    int i = startOffset;

    do {

        // Assign colour to nodes in the maximal independent set
        colors[iter->first] = colour;

        //Remove coloured nodes from uncolouredNodes, and from neighbours in uncolouredNodes
        //Encased in a mutex to ensure atomicity
        lpmutex.lock();
        uncoloredNodes.erase(iter->first);
        for (auto& neighbor : iter->second) {
            uncoloredNodes[neighbor].remove(iter->first);
        }
        lpmutex.unlock();
        
        i += stepSize;
        //Go to next node of thread
        if (i >= maximalIndependentSet.size()) {
            break;
        }
        advance(iter, stepSize);
    } while (i < maximalIndependentSet.size());

}

void assignRandomNumbersIters(map<int, list<int>>& graph, map<int, int>& graphNumberMap) {

    int maxThreads = thread::hardware_concurrency();

    vector<thread> workers;

    int stepSize = graph.size() / maxThreads;

    if (graph.size() >= maxThreads) {
        for (int i = 0; i < maxThreads; i++) {

            //Create thread for set of uncoloured nodes
            workers.emplace_back([&graph, &graphNumberMap, i, maxThreads] {assignNumberWorkerIters(graph, graphNumberMap, i, maxThreads); });

        }

        //Wait for threads to finish before moving to next step
        for (auto& worker : workers) {
            worker.join();
        }
    }
    else {
        thread worker([&graph, &graphNumberMap] {assignNumberWorkerIters(graph, graphNumberMap, 0, 1); });
        worker.join();
    }
    
}

void assignNumberWorkerIters(map<int, list<int>>& graph, map<int, int>& graphNumberMap, int startOffset, int stepSize) {
    // Step through the nodes with the step size of number of threads
    map<int, list<int>>::iterator iter = graph.begin();
    advance(iter, startOffset);

    int i = startOffset;

    do {

        graphNumberMap[iter->first] = rand();
        
        i += stepSize;
        //Go to next node of thread
        if (i >= graph.size()) {
            break;
        }
        advance(iter, stepSize);
    } while (i < graph.size());

}
