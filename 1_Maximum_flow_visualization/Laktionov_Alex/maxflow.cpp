#include "MaxFlow.h"

MaxFlow::MaxFlow(Graph g_input) {
    Change step = {BEGIN, 0, 0};
    steps.push_back(step);
    //g.setGraph(g_input.getVertexNumber(), g_input.getEdges());
    g = g_input;
    vertexNumber = g.getVertexNumber();
    adjacencyMatrix.resize( vertexNumber );
    Network::NetEdge netEdge;
    Graph::Edge edge;
    for ( size_t i = 0; i < g.getEdgesNumber(); ++i ) {
        edge = g.getEdge(i);
        netEdge.from = edge.from;
        netEdge.to = edge.to;
        netEdge.capacity = edge.weight;
        netEdge.flow = 0;
        network.edges.push_back( netEdge );
        adjacencyMatrix[edge.from].push_back( 2 * i );
        swap( netEdge.from, netEdge.to );
        netEdge.capacity = 0;
        network.edges.push_back( netEdge );
        adjacencyMatrix[edge.to].push_back( (2 * i) + 1);
    }
}

int MaxFlow::findMaxFlow() {
    push_relabel_worker();
    return maxFlowValue;
}

Network MaxFlow::getNetwork() const{
    Network answer;
    for ( size_t i = 0; i < network.edges.size(); i += 2 ) {
        answer.edges.push_back( network.edges[i] );
    }
    return answer;
}

vector<MaxFlow::Change> MaxFlow::getSteps(){
    return steps;
}

inline size_t MaxFlow::numberOfBackEdge( size_t edgeNumber ) {
    if (edgeNumber % 2 == 0)
        return edgeNumber + 1;
    else
        return edgeNumber - 1;
}


void MaxFlow::push( Network::NetEdge &edge,
                    Network::NetEdge &backedge ,
                    size_t edgeNumber)
{
    size_t u = edge.from;
    int delta = min( excess[u], network.residualWeight(edge) );

    edge.flow += delta;
    backedge.flow -= delta;

    excess[edge.from] -= delta;
    excess[edge.to] += delta;


    //передать инфу в Drawer
    Change step = {FLOW, edgeNumber, delta};
    steps.push_back(step);
    size_t backEdgeNumber = numberOfBackEdge(edgeNumber);
    step = {FLOW, backEdgeNumber, -delta};
    steps.push_back(step);
    step = {EXCESS, edge.from, -delta};
    steps.push_back(step);
    step = {EXCESS, edge.to, delta};
    steps.push_back(step);
}


void MaxFlow::relabel( size_t u ) {
    int min_val = (2 * vertexNumber) + 1;
    size_t currentEdge;
    for (size_t i = 0; i < adjacencyMatrix[u].size(); ++i ) {
        currentEdge = adjacencyMatrix[u][i];
        if ( network.residualWeight( currentEdge ) > 0)
            min_val = min( height[network.edges[currentEdge].to], min_val );
    }
    int delta = -height[u];
    height[u] = min_val + 1;
    delta += height[u]; // new - old

    //передать инфу в Drawer
    Change step = {HEIGHT, u, delta};
    steps.push_back(step);
}

bool MaxFlow::isPushable( size_t currentEdge, size_t u, size_t v ) {
    return network.residualWeight(currentEdge) > 0 && height[u] == height[v]+1;
}


void MaxFlow::discharge( size_t u ) {
    size_t v, currentEdge;
    while ( excess[u] > 0 ) {
        if ( adjacencyListPosition[u] < adjacencyMatrix[u].size() ) {
            v = network.edges[adjacencyMatrix[u][adjacencyListPosition[u]]].to;
            currentEdge = adjacencyMatrix[u][adjacencyListPosition[u]];
            if ( isPushable( currentEdge, u, v ) ) {
                push( network.edges[currentEdge],
                      network.edges[numberOfBackEdge(currentEdge)] ,
                      currentEdge);
            } else {
                adjacencyListPosition[u]++;
            }
        } else {
            relabel( u );
            adjacencyListPosition[u] = 0;
        }
    }
}

void MaxFlow::initPreFlow() {
    Change step;
    height.assign( vertexNumber, 0 );
    height[0] = vertexNumber;
    step = {HEIGHT, 0, vertexNumber};
    steps.push_back(step);
    excess.assign( vertexNumber, 0 );
    adjacencyListPosition.assign( vertexNumber, 0 );
    excess[0] = numeric_limits<int>::max();
    step = {EXCESS, 0, excess[0]};
    steps.push_back(step);
    for ( size_t i = 0; i < network.edges.size(); i += 2 ) {
        if ( network.edges[i].from == 0 ) {
            network.edges[i].flow += network.edges[i].capacity;
            step = {FLOW, i, network.edges[i].capacity};
            steps.push_back(step);
            network.edges[i+1].flow += -network.edges[i].capacity;
            step = {FLOW, i+1, -network.edges[i].capacity};
            steps.push_back(step);
            excess[network.edges[i].to] += network.edges[i].capacity;
            step = {EXCESS, network.edges[i].to, network.edges[i].capacity};
            steps.push_back(step);
            excess[0] -= network.edges[i].capacity;
            step = {EXCESS, 0, -network.edges[i].capacity};
            steps.push_back(step);
        }
    }
}

void MaxFlow::push_relabel_worker() {

    initPreFlow();


    list<size_t> processingVertices;
    for ( size_t i = 1; i < vertexNumber - 1; ++i )
        processingVertices.push_back( i );

    list<size_t>::iterator iter = processingVertices.begin();
    size_t oldHeight;

    while ( iter != processingVertices.end() ) {
        size_t u = *iter;
        oldHeight = height[u];
        discharge( u );

        if (height[u] > oldHeight) {
            processingVertices.erase( iter );
            iter = processingVertices.begin();
            processingVertices.push_front( u );
        } else
            iter++;
    }
    maxFlowValue = excess[vertexNumber - 1];
    Change step = {END, 0, 0};
    steps.push_back(step);
}
