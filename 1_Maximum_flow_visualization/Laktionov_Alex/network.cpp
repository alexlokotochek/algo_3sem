#include "Network.h"


int Network::residualWeight ( size_t edgeNum ) {
    return edges[edgeNum].capacity - edges[edgeNum].flow;
}

int Network::residualWeight ( NetEdge e ) {
    return e.capacity - e.flow;
}
