//
// Created by tommy on 19/08/2021.
//

#ifndef REPO_UTIL_H
#define REPO_UTIL_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stdio.h>
#include <list>
#include <ctime>
#include <chrono>

using namespace std;

map<int, list<int>> readGraph(string path, map<int, int> &graphNumberMap, map<int, list<int>> & randToNodesAssignedMap);

vector<int> initializeLabels(int size);

void parametersSetup(string &selectedAlg, int &nThreads, bool &menuMode, string &selectedGraph, string &finalPath, int argc, char ** argv, vector<string> algorithms, vector<string> graphPaths, string basePath);

bool prerunSetup(vector<int> &colors, int &alg, bool menuMode, vector<string> algorithms, int nThreads, string selectedGraph, int argc,
                 char **argv, map<int, list<int>> graph);

#endif //REPO_UTIL_H
