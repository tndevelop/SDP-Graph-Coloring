//
// Created by Marco Pappalardo on 30/08/2021.
//
#include "algorithmSDL.h"
#include <thread>
#include <iostream>
#include <mutex>
#include <future>
#include <vector>
#include <pthread.h>
using namespace std;

mutex sdlmutex, weightMutex, sdlmutex2, mutex3;
unique_lock<mutex> lockko(weightMutex), lockko2(mutex3);
condition_variable semSDL, semSDL2;
int val = 1, cons = 0, readers = 0, writers = 0;

int sdlSemaphore, sdlSemaphore2;

vector<int> smallestDegreeLastSequentialAssignment(map<int, list<int>> graph, vector<int> colors, int *maxColUsed) {
    int i = 1;
    int k = 0;
    map<int, list<int>> unweightedGraph(graph);
    map<int, list<int>> nodeWeights = {};
    map<int, int> graphNumberRandMap = {};
    vector<int> toBeRemoved = {};
    chrono::time_point<chrono::system_clock> startTime = chrono::system_clock::now();
    while (!unweightedGraph.empty()) {
        for (auto const &node : unweightedGraph) {
            if (node.second.size() <= k) {
                nodeWeights[i].emplace_back(node.first);
                graphNumberRandMap[node.first] = rand() % (graph.size() * 2);
                for (const auto neighbour : node.second) {
                    unweightedGraph[neighbour].remove(node.first);
                }
                toBeRemoved.emplace_back(node.first);
            }
        }
        if (toBeRemoved.size() > 0) {
            i++;
            for (auto node : toBeRemoved) {
                unweightedGraph.erase(node);
            }
            toBeRemoved.clear();
        }

        k++;
    }
    chrono::time_point<chrono::system_clock> endTime = chrono::system_clock::now();
    cout << "Time taken for weighting: " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count()
         << " milliseconds" << endl;

    map<int, list<int>> uncoloredNodes = graph;

    while (uncoloredNodes.size() > 0) {

        //Create independent set of vertices with the highest weights of all neighbours
        // map<int, list<int>> independentSet{};
        //       findIndependentSets(uncoloredNodes, graphNumberMap, graphNumberRandMap, independentSet);

        //In each independent set assign the minimum colour not belonging to a neighbour
        assignColoursSDL(uncoloredNodes, colors, nodeWeights, maxColUsed);

    }

    return colors;


}

void assignColoursSDL(map<int, list<int>> &uncoloredNodes, vector<int> &colors, map<int, list<int>> &nodeWeights,
                      int *maxColUsed) {
    //In each independent set assign the minimum colour not belonging to a neighbour
    map<int, list<int>> neighborColors = {};
    for (int i = nodeWeights.size(); i > 0; i--) {
        for (const auto node: nodeWeights[i]) {
            neighborColors[node].sort();

            //Build set of neighbours colours

            //assign lowest color not in use by any neighbor
            int selectedCol = 0;
            int previous = 0;
            for (auto neighborCol : neighborColors[node]) {
                if (neighborCol - previous > 1) {
                    selectedCol = previous + 1;
                    break;
                } else {
                    selectedCol++;
                }
                previous = neighborCol;
            }
            colors[node] = selectedCol;
            if (selectedCol > *maxColUsed)
                *maxColUsed = selectedCol;


            //Remove coloured nodes from uncolouredNodes, and from neighbours in uncolouredNodes
            //Encased in a mutex to ensure atomicity
            sdlmutex.lock();
            for (auto &neighbor : uncoloredNodes[node]) {
                uncoloredNodes[neighbor].remove(node);
                neighborColors[neighbor].emplace_back(selectedCol);
            }
            uncoloredNodes.erase(node);
            sdlmutex.unlock();

        }
    }

}

vector<int> smallestDegreeLastParallelAssignment(map<int, list<int>> graph, vector<int> colors, int *maxColUsed) {
    vector<thread> workers;
    int maxThreads = thread::hardware_concurrency();
    sdlSemaphore = 0;
    sdlSemaphore2 = 0;
    vector<int> toBeRemoved = {};
    map<int, list<int>> unweightedGraph(graph);
    map<int, list<int>> nodeWeights = {};
    chrono::time_point<chrono::system_clock> startTime = chrono::system_clock::now();
    for (int j = 0; j < maxThreads; j++) {

        workers.emplace_back([j, &unweightedGraph, &nodeWeights, maxThreads, &toBeRemoved] {
            parallelWeighting(j, unweightedGraph, nodeWeights, maxThreads, toBeRemoved);
        });

    }
    for (auto &thread : workers) {
        thread.join();
    }
    workers.clear();


    chrono::time_point<chrono::system_clock> endTime = chrono::system_clock::now();
    cout << "Time taken for weighting: " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count()
         << " milliseconds" << endl;

    map<int, list<int>> uncoloredNodes = graph;
    while (uncoloredNodes.size() > 0) {

        //Create independent set of vertices with the highest weights of all neighbours
        // map<int, list<int>> independentSet{};
        //       findIndependentSets(uncoloredNodes, graphNumberMap, graphNumberRandMap, independentSet);

        //In each independent set assign the minimum colour not belonging to a neighbour
        assignColoursSDL(uncoloredNodes, colors, nodeWeights, maxColUsed);
    }

    return colors;
}


void
parallelWeighting(int threadN, map<int, list<int>> &unweightedGraph, map<int, list<int>> &nodeWeights, int maxThreads,
                  vector<int> toBeRemoved) {
    map<int, list<int>>::iterator iter;
    int numThreads = maxThreads;


    do{
        int i = threadN;
        iter = unweightedGraph.begin();
        advance(iter, threadN);
        do {
            while (!writers == 0);
            sdlmutex.lock();
            readers++;
            sdlmutex.unlock();
            if (iter->second.size() <= cons) {
                nodeWeights[cons].emplace_back(iter->first);
                toBeRemoved.emplace_back(iter->first);
                readers--;
                sdlmutex.lock();
                writers++;
                sdlmutex.unlock();
                while(!readers==0);
                for (auto neighbor:iter->second) {
                    unweightedGraph[neighbor].remove(iter->first);
                }
                writers--;
                sdlmutex.unlock();
                sdlmutex2.lock();
                if (toBeRemoved.size() > 0) {
                    val++;
                    for (auto node : toBeRemoved) {
                        unweightedGraph.erase(node);
                    }
                    toBeRemoved.clear();
                    sdlmutex2.unlock();
                } else sdlmutex2.unlock();

            }else readers--;

            i += numThreads;
            if (i >= unweightedGraph.size()) {
                if(sdlSemaphore==maxThreads-1){
                    cons++;
                    sdlSemaphore=0;
                    semSDL.notify_all();
                }
                else{
                    sdlSemaphore++;
                    semSDL.wait(lockko);
                }
                break;
            }
            advance(iter, numThreads);
        } while (i < unweightedGraph.size());

    }while (!unweightedGraph.empty()) ;



}