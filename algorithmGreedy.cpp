#include "algorithmGreedy.h"

using namespace std;

vector<int> greedyAssignment(map<int, list<int>> graph, vector<int> colors, int *maxColUsed) {
    vector<int> neighborColors;
    int selectedCol;
    int percentage = graph.size() / 100;
    int count = 0;

    for (auto const& node : graph)
    {
        /*
        count++;
        if (percentage != 0 && count % percentage == 0)
            cout << count / percentage << "%" << endl;
        */
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
