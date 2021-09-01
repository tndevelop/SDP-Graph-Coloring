//
// Created by tommy on 01/09/2021.
//

#ifndef REPO_ALGORITHMLDF_H
#define REPO_ALGORITHMLDF_H

#include <map>
#include <vector>
#include <algorithm>
#include <list>
#include <thread>
#include <iostream>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <chrono>

using namespace std;

vector<int> ldfParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed, int nThreads);

map<int, int> assignDegree(map<int, list<int>> &map, int threads);

void findDegreeThread(map<int, list<int>>& uncoloredNodes, map<int, int>& graphNumberMap, int threadId, int maxThreads, int stepSize, int size);

void ldfVertexColouring(map<int, list<int>>& graph, map<int, int>& graphNumberMap, vector<int>& colors, int startOffset, int stepSize, int* maxColUsed, int* coloredNodes);

#endif //REPO_ALGORITHMLDF_H
