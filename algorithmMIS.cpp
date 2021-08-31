#include "algorithmMIS.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <future>
#include <vector>
#include <condition_variable>
#include <chrono>

using namespace std;

mutex mismutex, barrierMutex;
condition_variable cvMISColor, cvMISNodes;
int semMISColor, semMISNodes;

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

//Parallel implementation of Maximal Independent Set algorithm to check functionality and show a baseline
vector<int> misParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed, int nThreads) {

    int colour = 0;

    //In the main thread assign a random number to each vertex
    chrono::time_point<chrono::system_clock> mappingStart = chrono::system_clock::now();

    map<int, int> graphNumberMap = {};
    assignRandomNumber(graph, graphNumberMap);

    chrono::time_point<chrono::system_clock> mappingEnd = chrono::system_clock::now();
    cout << "Time taken to assign random number map: " << chrono::duration_cast<chrono::milliseconds>(mappingEnd - mappingStart).count() << " milliseconds" << endl;

    map<int, list<int>> uncoloredNodes = graph;

    //Get maximum number of threads for the system
    int maxThreads = (nThreads <= 0 || nThreads > thread::hardware_concurrency()) ? thread::hardware_concurrency() : nThreads ;
    cout << "Max threads supported: " << maxThreads << endl;
    semMISColor = maxThreads;
    semMISNodes = maxThreads;

    vector<thread> workers;

    for (int i = 0; i < maxThreads; i++) {

        //Create threads to find and colour independent sets
        workers.emplace_back([&uncoloredNodes, &graphNumberMap, &colors, &colour, i, maxThreads] {findAndColourIndependentSetsMIS(uncoloredNodes, graphNumberMap, colors, &colour, i, maxThreads); });

    }

    //Wait for all threads to finish
    for (auto& worker : workers) {
        worker.join();
    }

    *maxColUsed = colour;

    return colors;
}

void findAndColourIndependentSetsMIS(map<int, list<int>>& uncoloredNodes, map<int, int>& graphNumberMap, vector<int>& colors, int* colour, int startOffset, int stepSize) {
    
    int numThreads = stepSize; 

    while (uncoloredNodes.size() > startOffset) {

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
                // Colour node
                mismutex.lock();
                colors[iter->first] = *colour;
                mismutex.unlock();
            }

            i += stepSize;
            //Go to next node of thread
            if (i >= uncoloredNodes.size()) {
                break;
            }
            advance(iter, stepSize);

        } while (i < uncoloredNodes.size());

        // Signal completion of colouring set
        {
            unique_lock<mutex> lock(mismutex);
            semMISColor--;
            if (semMISColor == 0) {
                // Go to next colour
                int nextCol = *colour + 1;
                *colour = nextCol;
                cvMISColor.notify_all();
            }
            else {
                // Wait here for other threads to finish colouring
                cvMISColor.wait(lock);
            }
        } 

        // Then remove nodes from uncoloured nodes       
        {
            unique_lock<mutex> lock(mismutex); 
            //Remove coloured nodes from uncolouredNodes, and from neighbours in uncolouredNodes
            //Encased in a mutex to ensure atomicity
            for (auto& node : independentSet) {
                uncoloredNodes.erase(node.first);
                for (auto& neighbor : node.second) {
                    uncoloredNodes[neighbor].remove(node.first);
                }
            }

            // Signal completion of node removal
            semMISNodes--;
            if (semMISNodes == 0) {
                if (uncoloredNodes.size() < numThreads) {
                    semMISNodes = uncoloredNodes.size();
                    semMISColor = uncoloredNodes.size();
                }
                else {
                    semMISNodes = numThreads;
                    semMISColor = numThreads;
                }  
                cvMISNodes.notify_all();
            }
            else {
                // Wait here for other threads to finish removing nodes
                cvMISNodes.wait(lock);
            }
        }

    }

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
        thread worker([&uncoloredNodes, &colors, &maximalIndependentSet, colour] {assignColoursWorkerIters(uncoloredNodes, colors, maximalIndependentSet, colour, 0, 1); });
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
        mismutex.lock();
        uncoloredNodes.erase(iter->first);
        for (auto& neighbor : iter->second) {
            uncoloredNodes[neighbor].remove(iter->first);
        }
        mismutex.unlock();

        i += stepSize;
        //Go to next node of thread
        if (i >= maximalIndependentSet.size()) {
            break;
        }
        advance(iter, stepSize);
    } while (i < maximalIndependentSet.size());

}

void assignRandomNumber(map<int, list<int>>& graph, map<int, int>& graphNumberMap) {

    for (auto const& node : graph) {
        graphNumberMap[node.first] = rand();
    }

}

void assignRandomNumbersIters(map<int, list<int>>& graph, map<int, int>& graphNumberMap) {

    int maxThreads =  thread::hardware_concurrency();

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

/*
// Implementation of Luby's version of the algorithm (in progress)
vector<int> lubyMISParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed) {

    int colour = 0;

    map<int, list<int>> uncoloredNodes = graph;

    //Get maximum number of threads for the system
    int maxThreads = thread::hardware_concurrency();
    cout << "Max threads supported: " << maxThreads << endl;

    while (uncoloredNodes.size() > 0) {

        int nodesLeft = uncoloredNodes.size();

        // Calculate maximal independent set
        map<int, list<int>> maxIndependentSet{};
        lubysAlgorithm();


        //In each independent set assign the current colour
        assignColoursParallel(uncoloredNodes, colors, maxIndependentSet, colour);

        // Go to the next colour
        colour++;
    }

    *maxColUsed = colour;

    return colors;
}

void lubysAlgorithm(map<int, list<int>>& nodes, map<int, list<int>>& I, int numThreads) {

    map<int, list<int>> ghat = nodes;

    while (ghat.size() > 0) {

        map<int, list<int>> X{};

        vector<thread> workers;

        for (int i = 0; i < numThreads; i++) {
            workers.emplace_back([&ghat, &X, i, numThreads] {firstStepLuby(ghat, X, i, numThreads); });

        }

        //
        for (auto& worker : workers) {
            worker.join();
        }



    }

}

void firstStepLuby(map<int, list<int>>& ghat, map<int, list<int>> &X, int startOffset, int stepSize) {

    // Step through the nodes with the step size of number of threads
    map<int, list<int>>::iterator iter = ghat.begin();
    advance(iter, startOffset);

    int i = startOffset;
    do {
        int degree = iter->second.size();
        if (degree == 0) {
            lubymutex.lock();
            X.emplace(*iter);
            lubymutex.unlock();
        }
        else {
            // Randomly choose to add node to X with probability 1/2d(v)
            double prob = 1 / (2 * degree);
            double choice = rand() / RAND_MAX;
            if (choice <= prob) {
                lubymutex.lock();
                X.emplace(*iter);
                lubymutex.unlock();
            }
        }

        i += stepSize;
        if (i >= ghat.size()) {
            break;
        }
        advance(iter, stepSize);

    } while (i < ghat.size());

}
*/
