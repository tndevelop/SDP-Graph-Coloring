#include "algorithmLDF.h"

using namespace std;

mutex ldfmutex;
condition_variable cvLDFColor;
int semLDFColor;

vector<int> ldfParallelAssignment(map<int, list<int>>& graph, vector<int> colors, map<int, int> nodesDegree, int* maxColUsed, int nThreads) {

    int maxThreads = (nThreads <= 0 || nThreads > thread::hardware_concurrency()) ? thread::hardware_concurrency() : nThreads ;
    cout << "Max threads supported: " << maxThreads << endl;
    semLDFColor = maxThreads;




    vector<thread> workers;
    int coloredNodes = 0;
    for (int i = 0; i < maxThreads; i++) {

        //Create threads to find and colour independent sets
        workers.emplace_back([&graph, &nodesDegree, &colors, i, maxThreads, &maxColUsed, &coloredNodes] {ldfVertexColouring(graph, nodesDegree, colors, i, maxThreads, maxColUsed, &coloredNodes); });

    }

    //Wait for all threads to finish
    for (auto& worker : workers) {
        worker.join();
    }

    return colors;

}


void ldfVertexColouring(map<int, list<int>>& graph, map<int, int>& nodesDegree, vector<int>& colors, int startOffset, int stepSize, int* maxColUsed, int* coloredNodes) {

    int numThreads = stepSize;

    while (*coloredNodes < colors.size()) {
        // Step through the nodes with the step size of number of threads
        map<int, list<int>>::iterator iter = graph.begin();
        advance(iter, startOffset);

        int i = startOffset;

        do {
            if (colors[iter->first] == -1) {

                //Find out if I'm the max weighted node amongst uncoloured neighbours
                bool maxNode = true;
                vector<int> neighbourColors = {};

                for (auto const& neighbour : iter->second) {
                    if (colors[neighbour] == -1) {
                        if (nodesDegree[iter->first] < nodesDegree[neighbour]) {
                            maxNode = false;
                            break;
                        }
                        else if (nodesDegree[iter->first] == nodesDegree[neighbour]) {
                            if (iter->first < neighbour) {
                                maxNode = false;
                                break;
                            }
                        }
                    }
                    else {
                        neighbourColors.push_back(colors[neighbour]);
                    }
                }

                if (maxNode) {
                    // Colour node
                    int colour = 0;
                    while (find(neighbourColors.begin(), neighbourColors.end(), colour) != neighbourColors.end()) {
                        /* neighborColors contains selectedCol */
                        colour++;
                    }
                    if (colour > *maxColUsed) {
                        *maxColUsed = colour;
                    }
                    colors[iter->first] = colour;
                    ldfmutex.lock();
                    *coloredNodes = *coloredNodes + 1;
                    ldfmutex.unlock();
                }

            }

            i += stepSize;
            //Go to next node of thread
            if (i >= graph.size()) {
                break;
            }
            advance(iter, stepSize);

        } while (i < graph.size());

        // Signal completion of colouring set
        {
            unique_lock<mutex> lock(ldfmutex);
            semLDFColor--;
            if (semLDFColor == 0) {
                semLDFColor = numThreads;
                cvLDFColor.notify_all();
            }
            else {
                // Wait here for other threads to finish colouring
                cvLDFColor.wait(lock);
            }
        }

    }

}


