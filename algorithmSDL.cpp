//
// Created by Marco Pappalardo on 30/08/2021.
//
#include "algorithmSDL.h"
#include <thread>
#include <future>
#include <vector>

using namespace std;
mutex sdlmutex;

vector<int> smallestDegreeLastSequentialAssignment(map<int, list<int>> graph, vector<int> colors, int *maxColUsed,
                                                   map<int, list<int>> nodeWeights) {

    map<int, list<int>> uncoloredNodes = graph;



    //Create independent set of vertices with the highest weights of all neighbours
    // map<int, list<int>> independentSet{};
    //       findIndependentSets(uncoloredNodes, graphNumberMap, graphNumberRandMap, independentSet);

    //In each independent set assign the minimum colour not belonging to a neighbour
    assignColoursSDL(uncoloredNodes, colors, nodeWeights, maxColUsed);


    return colors;


}

void assignColoursSDL(map<int, list<int>> &uncoloredNodes, vector<int> &colors, map<int, list<int>> nodeWeights,
                      int *maxColUsed) {
    map<int, list<int>> neighborColors = {};
    // loop through all assigned weights (from the mximum to the minimun) to color nodes
    for (int i = nodeWeights.size(); i > 0; i--) {
        for (const auto node: nodeWeights[i]) {
            neighborColors[node].sort();
            int selectedCol = 0;
            int previous = 0;
            for (auto neighborCol : neighborColors[node]) // look for the lowest unassigned color
                 {
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

            for (auto &neighbor : uncoloredNodes[node]) {
                neighborColors[neighbor].emplace_back(selectedCol);
            }

        }
    }

}

void assignColoursSDLParallel(map<int, list<int>> &uncoloredNodes, vector<int> &colors, map<int, list<int>> nodeWeights,
                              int maxThread, int threadN,
                              int *maxColUsed) {
    map<int, list<int>> neighborColors = {};
    for (int i = nodeWeights.size() - threadN; i > threadN; i -= maxThread) {
        for (const auto node: nodeWeights[i]) {
            sdlmutex.lock();
            neighborColors[node].sort();
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
            sdlmutex.unlock();

            colors[node] = selectedCol;
            if (selectedCol > *maxColUsed)
                *maxColUsed = selectedCol;
            sdlmutex.lock();

            for (auto &neighbor : uncoloredNodes[node]) {
                neighborColors[neighbor].emplace_back(selectedCol);
            }
            sdlmutex.unlock();

        }
    }

}

vector<int> smallestDegreeLastParallelAssignment(map<int, list<int>> graph, vector<int> colors, int *maxColUsed,
                                                 map<int, list<int>> &nodeWeights) {
    vector<thread> workers;
    int maxThreads = thread::hardware_concurrency();

    for (int j = 0; j < maxThreads; j++) {

        workers.emplace_back([&graph, &colors, nodeWeights, maxThreads, j, maxColUsed] {
            assignColoursSDLParallel(graph, colors, nodeWeights, maxThreads, j,
                                     maxColUsed);
        });

    }
    for (auto &thread : workers) {
        thread.join();
    }
    workers.clear();

    return colors;


}



