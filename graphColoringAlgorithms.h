#ifndef REPO_GRAPHCOLORINGALGORITHMS_H
#define REPO_GRAPHCOLORINGALGORITHMS_H

#include <map>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> greedyAssignment(map<int, vector<int>> map, vector<int> vector, int *maxColUsed);

#endif //REPO_GRAPHCOLORINGALGORITHMS_H
