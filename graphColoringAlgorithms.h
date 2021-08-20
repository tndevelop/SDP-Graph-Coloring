#ifndef REPO_GRAPHCOLORINGALGORITHMS_H
#define REPO_GRAPHCOLORINGALGORITHMS_H

#include <map>
#include <vector>
#include <algorithm>
#include <list>

using namespace std;

vector<int> greedyAssignment(map<int, list<int>> map, vector<int> vector, int *maxColUsed);

vector<int> jonesPlassmannSequentialAssignment(map<int, list<int>> map, vector<int> vector, int* maxColUsed);

#endif //REPO_GRAPHCOLORINGALGORITHMS_H
