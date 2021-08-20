#include "graphColoringAlgorithms.h"

using namespace std;

vector<int> greedyAssignment(map<int, list<int>> graph, vector<int> colors, int *maxColUsed) {

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

//Starting with a sequential implementation of Jones-Plassman algorithm to check functionality and show a baseline
vector<int> jonesPlassmannSequentialAssignment(map<int, list<int>> graph, vector<int> colors, int* maxColUsed) {

    //Assign a random number to each vertex
    map<int, int> graphNumberMap = {};

    for (auto const& node : graph) {
        graphNumberMap[node.first] = rand();
    }

    map<int, list<int>> uncoloredNodes = graph;

    while (uncoloredNodes.size() > 0) {

        //Create independent set of vertices with the highest weights of all neighbours
        map<int, list<int>> independentSet{};
        for (auto const& node : uncoloredNodes) {
            bool maxNode = true;
            for (auto const& neighbour : node.second) {
                if (graphNumberMap[node.first] < graphNumberMap[neighbour]) {
                    maxNode = false;
                }
                else if (graphNumberMap[node.first] == graphNumberMap[neighbour]) {
                    if (node.first < neighbour) {
                        maxNode = false;
                    }
                }
            }
            if (maxNode) {
                independentSet.emplace(node);
            }
        }

        //In each independent set assign the minimum colour not belonging to a neighbour
        for (auto const& node : independentSet) {

            //Build set of neighbours colours
            vector<int> neighborColors = {};
            for (auto& neighbor : graph.at(node.first)) {
                if (colors[neighbor] != -1)
                    neighborColors.push_back(colors[neighbor]);
            }

            //assign lowest color not in use by any neighbor
            int selectedCol = 0;
            while (colors[node.first] == -1) {
                if (find(neighborColors.begin(), neighborColors.end(), selectedCol) != neighborColors.end()) {
                    /* neighborColors contains selectedCol */
                    selectedCol++;
                }
                else {
                    colors[node.first] = selectedCol;
                    if (selectedCol > *maxColUsed)
                        *maxColUsed = selectedCol;
                }
            }

            //Remove coloured nodes from uncolouredNodes, and from neighbours in uncolouredNodes
            uncoloredNodes.erase(node.first);
            for (auto& neighbor : node.second) {
                uncoloredNodes[neighbor].remove(node.first);
            }
        }

    }

    return colors;
}
