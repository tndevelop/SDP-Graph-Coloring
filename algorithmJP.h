#ifndef REPO_ALGORITHMJP_H
#define REPO_ALGORITHMJP_H

#include <map>
#include <vector>
#include <algorithm>
#include <list>

using namespace std;

vector<int> jonesPlassmannSequentialAssignment(map<int, list<int>>& graph, map<int, int>& graphNumberMap, vector<int> colors, int* maxColUsed);

vector<int> jonesPlassmannParallelAssignment(map<int, list<int>>& graph, map<int, int>& graphNumberMap, vector<int> colors, int* maxColUsed, int nThreads);
void jpVertexColouring(map<int, list<int>>& graph, map<int, int>& graphNumberMap, vector<int>& colors, int startOffset, int stepSize, int* maxColUsed, int* coloredNodes);
//void jpVertexColouring(map<int, list<int>>& uncoloredNodes, map<int, list<int>>& uncoloredNodesTemp, map<int, int>& graphNumberMap, vector<int>& colors, int startOffset, int stepSize, int* maxColUsed);

#endif //REPO_ALGORITHMJP_H