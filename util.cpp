#include "util.h"
#include <list>
#include <iostream>
#include <fstream>

using namespace std;

vector<int> initializeLabels( int size) {

     vector <int> labels (size, -1);
/*
     vector<int> labels;
     for (int i = 0; i< size; i++){
        labels.push_back(-1);
    }*/
    return labels;
}

map<int, list<int>> readGraph(string path, map<int, int> &graphNumberMap, map<int, list<int>> & randToNodesAssignedMap, map<int, int> &nodesDegree){

    chrono::time_point<chrono::system_clock> startTime = chrono::system_clock::now();
    cout << "Initializing reading process" << endl;
    string line;
    int nodeNumber=-1;
    list<int> linkedNodes;
    list<int> tmp;
    map<int, list<int>> graph {};
    int lineCount = 0;

    ifstream myfile (path);
    if (myfile.is_open())
    {
        //get number of nodes, which we actually don't need since we use a map
        getline (myfile,line);
        if(line == "graph_for_greach") getline (myfile,line);

        while ( getline (myfile,line) )
        {
            lineCount++;
            if(lineCount % 100000 == 0)
                cout << "line: " << lineCount << endl;
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
            graph[nodeNumber].insert(graph[nodeNumber].end(), linkedNodes.begin(), linkedNodes.end());

            //link back all the linked nodes to the current node
            for( auto & node : linkedNodes){
                graph[node].insert(graph[node].begin(), nodeNumber);
            }
        }
        myfile.close();
    }else cout << "Unable to open file";

    graphNumberMap = {};
    nodesDegree = {};

    for (auto const& node : graph) {
        int randNumber=rand()%(graph.size()*2);
        graphNumberMap[node.first] = randNumber;
        randToNodesAssignedMap[randNumber].insert(randToNodesAssignedMap[randNumber].begin(), node.first);
    }

    nodesDegree = assignDegree(graph, thread::hardware_concurrency());

    chrono::time_point<chrono::system_clock> endTime = chrono::system_clock::now();
    cout << "Time taken by Read: " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;

    return graph;
}


map<int, list<int>> readGraph2(string path, map<int, int> &graphNumberMap, map<int, list<int>> & randToNodesAssignedMap, map<int, int> &nodesDegree){

    chrono::time_point<chrono::system_clock> startTime = chrono::system_clock::now();
    cout << "Initializing reading process" << endl;
    string line;
    int nodeNumber=-1;
    list<int> linkedNodes;
    list<int> tmp;
    map<int, list<int>> graph {};
    int lineCount = 0;

    ifstream myfile (path);
    if (myfile.is_open())
    {
        //get number of nodes, which we actually don't need since we use a map
        getline (myfile,line);
        if(line == "graph_for_greach") getline (myfile,line);

        while ( getline (myfile,line) )
        {
            lineCount++;
            if(lineCount % 10000 == 0)
                cout << "line: " << lineCount << endl;
            linkedNodes = {};
            tmp = {};
            string delimiter = " ";
            size_t pos = 0;
            string token;
            nodeNumber = lineCount;
            //read all linked nodes
            while ((pos = line.find(delimiter)) != string::npos) {
                token = line.substr(0, pos);
                line.erase(0, pos + delimiter.length());
                linkedNodes.push_back(stoi(token));

            }
            linkedNodes.push_back(stoi(line));//get last neighbor
            //insert linked nodes into graph
            graph[nodeNumber].insert(graph[nodeNumber].end(), linkedNodes.begin(), linkedNodes.end());

            //link back all the linked nodes to the current node
            for( auto & node : linkedNodes){
                graph[node].insert(graph[node].begin(), nodeNumber);
            }
        }
        myfile.close();
    }else cout << "Unable to open file";

    graphNumberMap = {};
    nodesDegree = {};

    for (auto const& node : graph) {
        int randNumber=rand()%(graph.size()*2);
        graphNumberMap[node.first] = randNumber;
        randToNodesAssignedMap[randNumber].insert(randToNodesAssignedMap[randNumber].begin(), node.first);
    }

    nodesDegree = assignDegree(graph, thread::hardware_concurrency());

    chrono::time_point<chrono::system_clock> endTime = chrono::system_clock::now();
    cout << "Time taken by Read: " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;

    return graph;
}

void parametersSetup(string &selectedAlg, int &nThreads, bool &menuMode, string &selectedGraph, string &finalPath, int argc, char ** argv, vector<string> algorithms, vector<string> graphPaths, string basePath) {
    selectedGraph = graphPaths[atoi(argv[1])];
    finalPath = basePath + selectedGraph;
    if(argc >= 3){

        selectedAlg = atoi(argv[2]) <= algorithms.capacity() ? algorithms[atoi(argv[2])] : "NONE";
        cout << endl << "-------------------------------------------------------------------------" << endl;
        cout << "running " << selectedAlg << " algorithm on graph " << selectedGraph << endl << endl ;
        if(argc >= 4){
            nThreads = atoi(argv[3]);
        }
    }else{
        menuMode = true;

        cout << "Welcome to the graph coloring program." << endl;
        cout << "You have selected menu mode, you will be asked to choose an algorithm shortly." << endl;
        cout << "Graph " << selectedGraph << " will be analyzed. " << endl ;
    }
}

bool prerunSetup(vector<int> &colors, int &alg, bool menuMode, vector<string> algorithms, int nThreads, string selectedGraph, int argc,
                 char **argv, map<int, list<int>> graph, map<int, list<int>> &nodeWeight ) {
    colors = initializeLabels(graph.size());
    int i=0;
    if(menuMode){
        cout << "Please select an algorithm: " << endl;
        for(const string &algorithm : algorithms) {
            cout << i << ": " << algorithms[i];
            (i%2 == 1 || i==algorithms.size()-1) ? cout << endl : cout << "\t\t";
            i++;
        }
        cout << "Insert any other number to exit" << endl;
        cin >> alg;
        //if(alg >= algorithms->size())
        if(alg >= algorithms.capacity())
            return false;

        if(!(alg == 0 || alg == 1 || alg == 3)) {
            cout << endl << "Select number of threads:" << endl;
            cin >> nThreads;
        }
        nThreads = (nThreads <= 0 || nThreads > thread::hardware_concurrency()) ? thread::hardware_concurrency() : nThreads ;
        cout << "Running " << algorithms[alg] << " algorithm on graph " << selectedGraph << " with " << nThreads << " threads" << endl << endl ;
    }else{
        alg = atoi(argv[2]);
    }
    if(alg==3 || alg==4){
        nodeWeight= assignWeight(graph);
    }
    return true;

}


map<int, int> assignDegree(map<int, list<int>>& graph, int maxThreads) {
    map<int, int> nodesDegree = {};

    vector<thread> workers;

    for (int i = 0; i < maxThreads; i++) {
        int size = graph.size();
        int stepSize = size/maxThreads;
        workers.emplace_back([&graph, &nodesDegree, i, maxThreads, stepSize, size] {findDegreeThread(graph, nodesDegree, i, maxThreads, stepSize, size); });
    }

    //Wait for all threads to finish
    for (auto& worker : workers) {
        worker.join();
    }
    return nodesDegree;
}

map<int,list<int>> assignWeight (map <int,list<int>> graph){
    int i = 1;
    int k = 0;
    map<int, list<int>> unweightedGraph(graph);
    map<int, list<int>> nodeWeights = {};
    vector<int> toBeRemoved = {};
    while (!unweightedGraph.empty()) {
        for (auto const &node : unweightedGraph) {
            if (node.second.size() <= k) {
                nodeWeights[i].emplace_back(node.first);
                for (const auto neighbour : node.second) {
                    unweightedGraph[neighbour].remove(node.first);
                }
                toBeRemoved.emplace_back(node.first);
            }
        }
        if (toBeRemoved.size() > 0) {
            i++;
            for (auto node : toBeRemoved) {
                unweightedGraph.erase(node);
            }
            toBeRemoved.clear();
        }
        k++;
    }
    return nodeWeights;
};

void findDegreeThread(map<int, list<int>>& graph, map<int, int>& nodesDegree, int threadId, int maxThreads, int stepSize, int size) {
    int lastIndx = threadId == maxThreads-1 ? size : threadId * stepSize + stepSize ;
    for(int i = threadId * stepSize; i < lastIndx; i++){
        nodesDegree[i] = graph[i].size();
    }
    return;
}

// Function to create a script file for running multiple graph/algorithm combinations
void createBatchFile() {

    vector<int> graphs = { 0, 1, 3, 5, 7, 8, 9, 10, 13};
    vector<int> threads = { 2, 4, 8 };

    ofstream file;
    file.open("runScript.cmd");

    for (int i = 0; i < graphs.size(); i++) {
        // Greedy
        file << "GraphColouring.exe " << graphs[i] << " " << 0 << " > cmd_out/graph_" << graphs[i] << "_greedy.txt" << endl;

        //JP
        file << "GraphColouring.exe " << graphs[i] << " " << 1 << " > cmd_out/graph_" << graphs[i] << "_JP_sequential.txt" << endl;
        for (int k = 0; k < threads.size(); k++) {
            file << "GraphColouring.exe " << graphs[i] << " " << 2 << " " << threads[k] << " > cmd_out/graph_" << graphs[i] << "_JP_parallel_threads_" << threads[k] << ".txt" << endl;
        }

        /*//SDL
        file << "GraphColouring.exe " << graphs[i] << " " << 3 << " > cmd_out/graph_" << graphs[i] << "_SDL_sequential.txt" << endl;
        for (int k = 0; k < threads.size(); k++) {
            file << "GraphColouring.exe " << graphs[i] << " " << 4 << " " << threads[k] << " > cmd_out/graph_" << graphs[i] << "_SDL_parallel_threads_" << threads[k] << ".txt" << endl;
        }*/

        //MIS
        file << "GraphColouring.exe " << graphs[i] << " " << 5 << " > cmd_out/graph_" << graphs[i] << "_MIS_sequential.txt" << endl;
        for (int k = 0; k < threads.size(); k++) {
            file << "GraphColouring.exe " << graphs[i] << " " << 6 << " " << threads[k] << " > cmd_out/graph_" << graphs[i] << "_MIS_parallel_threads_" << threads[k] << ".txt" << endl;
        }

        //LDF
        file << "GraphColouring.exe " << graphs[i] << " " << "7 1" << " > cmd_out/graph_" << graphs[i] << "_LDF_sequential.txt" << endl;
        for (int k = 0; k < threads.size(); k++) {
            file << "GraphColouring.exe " << graphs[i] << " " << 7 << " " << threads[k] << " > cmd_out/graph_" << graphs[i] << "_LDF_parallel_threads_" << threads[k] << ".txt" << endl;
        }
        
    }

    file.close();
}
