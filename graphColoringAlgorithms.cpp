
#include "graphColoringAlgorithms.h"

using namespace std;

vector<int> greedyAssignment(map<int, vector<int>> graph, vector<int> colors, int *maxColUsed) {

    vector<int> neighborColors;
    int selectedCol;

    for (auto const& node : graph)
    {
        neighborColors = {};
        selectedCol = 0;

        //find all neighbors' colours
        for (auto & neighbor : node.second) {
            if (colors[neighbor] != -1)
                neighborColors.push_back(colors[neighbor]);
        }

        //assign lowest color not in use by any neighbor
        while(colors[node.first]== -1){
            if(find(neighborColors.begin(), neighborColors.end(), selectedCol) != neighborColors.end()) {
                /* neighborColors contains selectedCol */
                selectedCol++;
            }else{
                colors[node.first] = selectedCol;
                if(selectedCol > *maxColUsed)
                    *maxColUsed = selectedCol;
            }

        }
    }

    return colors;

}
