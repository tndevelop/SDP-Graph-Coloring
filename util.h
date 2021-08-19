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


using namespace std;

map<int, vector<int>> readGraph(string path);

vector<int> initializeLabels(int size);

#endif //REPO_UTIL_H
