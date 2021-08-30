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

void findIndependentSetsMISIters(map<int, list<int>>& uncoloredNodes, map<int, int>& graphNumberMap, map<int, list<int>>& maxIndependentSet, int startOffset, int stepSize);
vector<int> misIteratorsParallelAssignment(map<int, list<int>>& graph, vector<int> colors, int* maxColUsed);
void assignColoursWorkerIters(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour, int startOffset, int stepSize);
void assignColoursParallelIters(map<int, list<int>>& uncoloredNodes, vector<int>& colors, map<int, list<int>>& maximalIndependentSet, int colour);
void assignNumberWorkerIters(map<int, list<int>>& graph, map<int, int>& graphNumberMap, int startOffset, int stepSize);
void assignRandomNumbersIters(map<int, list<int>>& graph, map<int, int>& graphNumberMap);
void findAndColourIndependentSetsMIS(map<int, list<int>>& uncoloredNodes, map<int, int>& graphNumberMap, vector<int>& colors, int* colour, int startOffset, int stepSize);

void assignRandomNumber(map<int, list<int>>& graph, map<int, int>& graphNumberMap);

#endif //REPO_ALGORITHMMIS_H