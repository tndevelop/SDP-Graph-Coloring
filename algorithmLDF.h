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

vector<int> ldfParallelAssignment(map<int, list<int>>& graph, vector<int> colors, map<int, int> nodesDegree, int* maxColUsed, int nThreads);

void ldfVertexColouring(map<int, list<int>>& graph, map<int, int>& nodesDegree, vector<int>& colors, int startOffset, int stepSize, int* maxColUsed, int* coloredNodes);

#endif //REPO_ALGORITHMLDF_H
