#include "util.h"
#include <list>

using namespace std;

vector<int> initializeLabels( int size) {
    vector<int> labels;
    for (int i = 0; i< size; i++){
        labels.push_back(-1);
    }
    return labels;
}

map<int, list<int>> readGraph(string path){
    string line;
    int nodeNumber=-1;
    list<int> linkedNodes;
    list<int> tmp;
    map<int, list<int>> graph {};

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
