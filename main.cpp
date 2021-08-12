#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stdio.h>
#include <algorithm>

using namespace std;

map<int, vector<int>> readGraph(string path);

vector<int> initializeLabels(int size);

vector<int> greedyAssignment(map<int, vector<int>> map, vector<int> vector);

int maxColUsed = -1;

int main() {

    //map<int, vector<int>> graph = readGraph("./graphs/benchmark/manual/v10.gra");
    //map<int, vector<int>> graph = readGraph("./graphs/benchmark/manual/v100.gra");
    //map<int, vector<int>> graph = readGraph("./graphs/benchmark/small_sparse_real/agrocyc_dag_uniq.gra");
    //map<int, vector<int>> graph = readGraph("./graphs/benchmark/small_sparse_real/human_dag_uniq.gra");
    //map<int, vector<int>> graph = readGraph("./graphs/benchmark/small_dense_real/arXiv_sub_6000-1.gra");
    //map<int, vector<int>> graph = readGraph("./graphs/benchmark/scaleFree/ba10k5d.gra");
    map<int, vector<int>> graph = readGraph("./graphs/benchmark/large/uniprotenc_22m.scc.gra");
    //map<int, vector<int>> graph = readGraph("./graphs/benchmark/large/go_uniprot.gra");

    vector<int> colors = initializeLabels(graph.size());

    colors = greedyAssignment(graph, colors);

    return 0;
}

vector<int> greedyAssignment(map<int, vector<int>> graph, vector<int> colors) {

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
            if(std::find(neighborColors.begin(), neighborColors.end(), selectedCol) != neighborColors.end()) {
                /* neighborColors contains selectedCol */
                selectedCol++;
            }else{
                colors[node.first] = selectedCol;
                if(selectedCol > maxColUsed)
                    maxColUsed = selectedCol;
            }

        }
    }

    return colors;

}

vector<int> initializeLabels( int size) {
    vector<int> labels;
    for (int i = 0; i< size; i++){
        labels.push_back(-1);
    }
    return labels;
}

map<int, vector<int>> readGraph(string path){
    string line;
    int nodeNumber=-1;
    vector<int> linkedNodes;
    vector<int> tmp;
    map<int, vector<int>> graph {};

    ifstream myfile (path);
    if (myfile.is_open())
    {
        //get number of nodes, which we actually don't need since we use a map
        getline (myfile,line);
        if(line == "graph_for_greach") getline (myfile,line);

        while ( getline (myfile,line) )
        {

            linkedNodes = {};
            tmp = {};
            string delimiter = ":";
            size_t pos = 0;
            string token;

            //read all linked nodes
            while ((pos = line.find(delimiter)) != string::npos) {
                token = line.substr(0, pos);
                line.erase(0, pos + delimiter.length());
                if(delimiter == ":") {
                    nodeNumber = stoi(token);
                    delimiter = " ";
                    line.erase(0, delimiter.length());
                }else if (delimiter != "#"){
                    linkedNodes.push_back(stoi(token));
                }
            }

            //insert linked nodes into graph
            if (graph.count(nodeNumber)) {
                //key already exists
                tmp = graph[nodeNumber];
                tmp.insert(tmp.end(), linkedNodes.begin(), linkedNodes.end());
                graph[nodeNumber] = tmp;
            }else{
                graph[nodeNumber] = linkedNodes;
            }

            //link back all the linked nodes to the current node
            for( auto & node : linkedNodes){

                if (graph.count(node)) {
                    //key already exists
                    tmp = graph[node];

                    tmp.insert(tmp.begin(), nodeNumber);
                    graph[node] = tmp;
                }else{
                    tmp={};
                    tmp.insert(tmp.begin(), nodeNumber);
                    graph[node] = tmp;
                }

            }
        }
        myfile.close();
    }else cout << "Unable to open file";

    return graph;
}