#include "algorithmJP.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <future>
#include <vector>
#include <condition_variable>
#include <chrono>

using namespace std;

mutex jpmutex;
condition_variable cvJPColor, cvJPNodes;
int semJPColor, semJPNodes;

//Parallel implementation of Jones-Plassman algorithm
vector<int> jonesPlassmannParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed) {

    //In the main thread assign a random number to each vertex
    chrono::time_point<chrono::system_clock> mappingStart = chrono::system_clock::now();

    map<int, int> graphNumberMap = {};
    for (auto const& node : graph) {
        graphNumberMap[node.first] = rand();
    }

    chrono::time_point<chrono::system_clock> mappingEnd = chrono::system_clock::now();
    cout << "Time taken to assign random number map: " << chrono::duration_cast<chrono::milliseconds>(mappingEnd - mappingStart).count() << " milliseconds" << endl;

    map<int, list<int>> uncoloredNodes = graph;

    //Get maximum number of threads for the system
    int maxThreads = thread::hardware_concurrency();
    cout << "Max threads supported: " << maxThreads << endl;
    semJPColor = maxThreads;
    semJPNodes = maxThreads;

    vector<thread> workers;

    for (int i = 0; i < maxThreads; i++) {

        //Create threads to find and colour independent sets
        workers.emplace_back([&uncoloredNodes, &graphNumberMap, &colors, i, maxThreads, &maxColUsed] {jpVertexColouring(uncoloredNodes, graphNumberMap, colors, i, maxThreads, maxColUsed); });

    }

    //Wait for all threads to finish
    for (auto& worker : workers) {
        worker.join();
    }

    //*maxColUsed = colour;

    return colors;
}

void jpVertexColouring(map<int, list<int>>& uncoloredNodes, map<int, int>& graphNumberMap, vector<int>& colors, int startOffset, int stepSize, int* maxColUsed) {
    
    int numThreads = stepSize; 

    while (uncoloredNodes.size() > startOffset) {

        map<int, list<int>> independentSet{};

        // Step through the nodes with the step size of number of threads
        map<int, list<int>>::iterator iter = uncoloredNodes.begin();
        advance(iter, startOffset);

        int i = startOffset;

        do {
            //Find out if I'm the max weighted node amongst uncoloured neighbours
            bool maxNode = true;
            vector<int> neighbourColors = {};

            for (auto const& neighbour : iter->second) {
                if (colors[neighbour] == -1) {
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
                else {
                    neighbourColors.push_back(colors[neighbour]);
                }
            }

            if (maxNode) {
                independentSet.emplace(*iter);
                // Colour node
                int colour = 0;
                while (find(neighbourColors.begin(), neighbourColors.end(), colour) != neighbourColors.end()) {
                    /* neighborColors contains selectedCol */
                    colour++;
                }
                if (colour > *maxColUsed) {
                    *maxColUsed = colour;
                }   
                jpmutex.lock();
                colors[iter->first] = colour;
                jpmutex.unlock();
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
            unique_lock<mutex> lock(jpmutex);
            semJPColor--;
            if (semJPColor == 0) {
                cvJPColor.notify_all();
            }
            else {
                // Wait here for other threads to finish colouring
                cvJPColor.wait(lock);
            }
        } 


        // Then remove nodes from uncoloured nodes       
        {
            unique_lock<mutex> lock(jpmutex);
            //Remove coloured nodes from uncolouredNodes
            //Encased in a mutex to ensure atomicity
            for (auto& node : independentSet) {
                uncoloredNodes.erase(node.first);
            }

            // Signal completion of node removal
            semJPNodes--;
            if (semJPNodes == 0) {
                if (uncoloredNodes.size() < numThreads) {
                    semJPNodes = uncoloredNodes.size();
                    semJPColor = uncoloredNodes.size();
                }
                else {
                    semJPNodes = numThreads;
                    semJPColor = numThreads;
                }
                cvJPNodes.notify_all();
            }
            else {
                // Wait here for other threads to finish removing nodes
                cvJPNodes.wait(lock);
            }
        }

    }

}

