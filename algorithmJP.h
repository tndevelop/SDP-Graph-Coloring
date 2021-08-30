#ifndef REPO_ALGORITHMJP_H
#define REPO_ALGORITHMJP_H

#include <map>
#include <vector>
#include <algorithm>
#include <list>

using namespace std;

vector<int> jonesPlassmannParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed);
void jpVertexColouring(map<int, list<int>>& uncoloredNodes, map<int, int>& graphNumberMap, vector<int>& colors, int startOffset, int stepSize, int* maxColUsed);

#endif //REPO_ALGORITHMJP_H