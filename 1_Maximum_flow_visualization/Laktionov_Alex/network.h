#ifndef NETWORK_H
#define NETWORK_H

#include "graph.h"

class Network {
public:

    struct NetEdge {
        size_t from;
        size_t to;
        int capacity;
        int flow;
    };

    vector<NetEdge> edges;

    int residualWeight ( size_t edgeNum );
    int residualWeight ( NetEdge e );
};

#endif // NETWORK_H
