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

void findIndependentSets(map<int, list<int>> &uncoloredNodes, map<int, int> &graphNumberMap,map<int, int> &graphNumberRandMap, map<int, list<int>>& independentSet);
void findIndependentSetsParallel(map<int, list<int>> &uncoloredNodes, map<int, int> &graphNumberMap, map<int, list<int>>& independentSet);

void assignColours(map<int, list<int>> &uncoloredNodes, vector<int> &colors, map<int, list<int>> &graph, map<int, list<int>> &independentSet, int* maxColUsed);
void assignColoursSDL(map<int, list<int>> &uncoloredNodes, vector<int> &colors,  map<int, list<int>> &nodeWeights, int* maxColUsed);

vector<int> smallestDegreeLastSequentialAssignment(map<int, list<int>> map, vector<int> vector, int* maxColUsed);
vector<int> smallestDegreeLastParallelAssignment(map<int, list<int>> map, vector<int> vector, int* maxColUsed);

map<int,int> weightNodes(map<int,list<int>> graph);
void parallelWeighting(pair<int, list<int>> node,map<int, int> &graphNumberMap, int i, map<int,list<int>> &unweightedGraph, map<int,list<int>> &nodeWeights, map<int, int> &graphNumberRandMap,  map<int,list<int>> &graph, map<int,list<int>> &tempGraph);
void findMaxNode(pair<int,list<int>> node,map<int, list<int>> &myUncoloredNodes,map<int, int> &graphNumberMap,map<int, int> &graphNumberRandMap, map<int, list<int>> &independentSet);

vector<int> misSequentialAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed);

void assignColoursMIS(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour);
void findMaximalIndependentSet(map<int, list<int>>& myUncoloredNodes, map<int, list<int>>& maximalIndependentSet);
void findIndependentSetsMIS(map<int, list<int>> myUncoloredNodes, map<int, int>& graphNumberMap, map<int, list<int>>& maximalIndependentSet);
vector<int> misParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed);
void assignColoursParallel(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour);
void assignColoursWorker(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>> maximalIndependentSet, int colour);


void findIndependentSetsMISIters(map<int, list<int>>& uncoloredNodes, map<int, int>& graphNumberMap, map<int, list<int>>& maxIndependentSet, int startOffset, int stepSize);
vector<int> misIteratorsParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed);
void assignColoursWorkerIters(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour, int startOffset, int stepSize);
void assignColoursParallelIters(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour);
void assignNumberWorkerIters(map<int, list<int>>& graph, map<int, int>& graphNumberMap, int startOffset, int stepSize);
void assignRandomNumbersIters(map<int, list<int>>& graph, map<int, int>& graphNumberMap);

#endif //REPO_GRAPHCOLORINGALGORITHMS_H
