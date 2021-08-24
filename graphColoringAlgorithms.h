#ifndef REPO_GRAPHCOLORINGALGORITHMS_H
#define REPO_GRAPHCOLORINGALGORITHMS_H

#include <map>
#include <vector>
#include <algorithm>
#include <list>

using namespace std;

vector<int> greedyAssignment(map<int, list<int>> map, vector<int> vector, int *maxColUsed);

vector<int> jonesPlassmannSequentialAssignment(map<int, list<int>> map, vector<int> vector, int* maxColUsed);

vector<int> jonesPlassmannParallelAssignment(map<int, list<int>> map, vector<int> vector, int* maxColUsed);

void workerFunction(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& graph, map<int, int>& graphNumberMap, int* maxColUsed, map<int, list<int>> nodesToColour);

map<int, list<int>> findIndependentSets(map<int, list<int>> uncoloredNodes, map<int, int> &graphNumberMap);

void assignColours(map<int, list<int>> &uncoloredNodes, vector<int> &colors, map<int, list<int>> &graph, map<int, list<int>> independentSet, int* maxColUsed);

#endif //REPO_GRAPHCOLORINGALGORITHMS_H
