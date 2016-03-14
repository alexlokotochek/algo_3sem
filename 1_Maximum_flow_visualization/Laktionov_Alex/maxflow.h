#ifndef MAXFLOW_H
#define MAXFLOW_H

#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <limits>
#include <utility>
#include "graph.h"
#include "maxflow.h"
#include "network.h"

class MaxFlow {
public:

    enum actionType{
        FLOW,
        HEIGHT,
        EXCESS,
        BEGIN,
        END // нулевой и последний шаг, для удобной индексации
    };

    struct Change {
        actionType what; // элемент enum - тип изменения
        size_t who; // номер ребра или вершины
        int delta; // new_value - old_value
    };

    Graph g;
    MaxFlow(Graph g_input);
    int findMaxFlow();
    Network getNetwork() const;
    vector<MaxFlow::Change> getSteps();

private:

    int vertexNumber;
    vector<vector<size_t>> adjacencyMatrix; // матрица смежности
    vector<size_t> currentEdgePointer, adjacencyListPosition;
    vector<int> height; // heights (labels)
    vector<int> excess; //excess of vertex
    vector<Change> steps;
    Network network;
    int maxFlowValue;

    inline size_t numberOfBackEdge( size_t edgeNum );
    void push( Network::NetEdge &edge, Network::NetEdge &backedge, size_t edgeNumber);
    void relabel( size_t u );
    bool isPushable( size_t currentEdge, size_t u, size_t v );
    void discharge( size_t u );
    void initPreFlow();
    void push_relabel_worker();
};

#endif // MAXFLOW_H
