#ifndef REPO_GRAPHCOLORINGALGORITHMS_H
#define REPO_GRAPHCOLORINGALGORITHMS_H

#include <map>
#include <vector>
#include <algorithm>
#include <list>

using namespace std;



vector<int> jonesPlassmannSequentialAssignment(map<int, list<int>> map, vector<int> vector, int* maxColUsed);

void assignNumberWorker(map<int, list<int>>& graph, map<int, int>& graphNumberMap);

void assignRandomNumbers(map<int, list<int>>& graph, map<int, int>& graphNumberMap, int nThreads);

void workerFunction(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& graph, map<int, int>& graphNumberMap, int* maxColUsed, map<int, list<int>> nodesToColour); //vector<map<int, list<int>>>& nodesToColourVect, int i); // 

void findIndependentSets(map<int, list<int>> &uncoloredNodes, map<int, int> &graphNumberMap,map<int, int> &graphNumberRandMap, map<int, list<int>>& independentSet);
void findIndependentSetsParallel(map<int, list<int>> &uncoloredNodes, map<int, int> &graphNumberMap, map<int, list<int>>& independentSet);

void assignColours(map<int, list<int>> &uncoloredNodes, vector<int> &colors, map<int, list<int>> &graph, map<int, list<int>> &independentSet, int* maxColUsed);

map<int,int> weightNodes(map<int,list<int>> graph);
void findMaxNode(pair<int,list<int>> node,map<int, list<int>> &myUncoloredNodes,map<int, int> &graphNumberMap,map<int, int> &graphNumberRandMap, map<int, list<int>> &independentSet);

void findIndependentSetsMIS(map<int, list<int>> myUncoloredNodes, map<int, int>& graphNumberMap, map<int, list<int>>& maximalIndependentSet);
vector<int> misParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed, int nThreads);
void assignColoursParallel(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour, int nThreads);
void assignColoursWorker(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>> maximalIndependentSet, int colour);

#endif //REPO_GRAPHCOLORINGALGORITHMS_H
