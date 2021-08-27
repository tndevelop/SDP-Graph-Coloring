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

void assignNumberWorker(map<int, list<int>>& graph, map<int, int>& graphNumberMap);

void assignRandomNumbers(map<int, list<int>>& graph, map<int, int>& graphNumberMap);

void workerFunction(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& graph, map<int, int>& graphNumberMap, int* maxColUsed, map<int, list<int>> nodesToColour); //vector<map<int, list<int>>>& nodesToColourVect, int i); // 

void findIndependentSets(map<int, list<int>> &uncoloredNodes, map<int, int> &graphNumberMap, map<int, list<int>>& independentSet);

void assignColours(map<int, list<int>> &uncoloredNodes, vector<int> &colors, map<int, list<int>> &graph, map<int, list<int>> &independentSet, int* maxColUsed);

vector<int> smallestDegreeLastSequentialAssignment(map<int, list<int>> map, vector<int> vector, int* maxColUsed);
vector<int> smallestDegreeLastParallelAssignment(map<int, list<int>> map, vector<int> vector, int* maxColUsed);

map<int,int> weightNodes(map<int,list<int>> graph);
void parallelWeighting(pair<int, list<int>> node, map<int, int> graphNumberMap, int i,  map<int,list<int>> unweightedGraph);

vector<int> misSequentialAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed);

void assignColoursMIS(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& graph, map<int, list<int>>& maximalIndependentSet, int colour);
void findMaximalIndependentSet(map<int, list<int>>& myUncoloredNodes, map<int, list<int>>& maximalIndependentSet);


#endif //REPO_GRAPHCOLORINGALGORITHMS_H
