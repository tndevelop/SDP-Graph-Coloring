//
// Created by Marco Pappalardo on 30/08/2021.
//

#ifndef REPO_ALGORITHMSDL_H
#define REPO_ALGORITHMSDL_H
#include <map>
#include <vector>
#include <algorithm>
#include <list>

using namespace std;

void assignColoursSDL(map<int, list<int>> &uncoloredNodes, vector<int> &colors,  map<int, list<int>> &nodeWeights, int* maxColUsed);

vector<int> smallestDegreeLastSequentialAssignment(map<int, list<int>> map, vector<int> vector, int* maxColUsed);
vector<int> smallestDegreeLastParallelAssignment(map<int, list<int>> map, vector<int> vector, int* maxColUsed);
void parallelWeighting(int threadN, map<int,list<int>> &unweightedGraph, map<int,list<int>> &nodeWeights,int maxThreads,vector<int> toBeRemoved);

#endif //REPO_ALGORITHMSDL_H
