#ifndef REPO_ALGORITHMMIS_H
#define REPO_ALGORITHMMIS_H

#include <map>
#include <vector>
#include <algorithm>
#include <list>

using namespace std;

vector<int> misSequentialAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed);
void assignColoursMIS(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour);
void findMaximalIndependentSet(map<int, list<int>>& myUncoloredNodes, map<int, list<int>>& maximalIndependentSet);

vector<int> misParallelAssignment(map<int, list<int>>& graph, map<int, int>& graphNumberMap, vector<int> colors, int* maxColUsed, int nThreads);
void findAndColourIndependentSetsMIS(map<int, list<int>>& graph, map<int, int>& graphNumberMap, vector<int>& colors, int* colour, int startOffset, int stepSize);

#endif //REPO_ALGORITHMMIS_H