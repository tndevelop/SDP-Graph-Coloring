#include "algorithmMIS.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <future>
#include <vector>
#include <condition_variable>
#include <chrono>

using namespace std;

mutex mismutex;
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
vector<int> misParallelAssignment(map<int, list<int>>& graph, map<int, int>& graphNumberMap, vector<int> colors, int* maxColUsed, int nThreads) {

    int colour = 0;

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
