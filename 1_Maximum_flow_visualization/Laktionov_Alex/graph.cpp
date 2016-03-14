#include "graph.h"

void Graph::setGraph(size_t vertexNumber_input, vector<Graph::Edge> edges_input){
    vertexNumber = vertexNumber_input;
    edges = edges_input;
}

void Graph::setGraphInfo(size_t inp_vertexNumber) {
    vertexNumber = inp_vertexNumber;
}

void Graph::setEdges(vector<Graph::Edge> input){
    edges = input;
}

Graph::Edge Graph::getEdge(size_t index) const{
    return edges[index];
}

size_t Graph::getEdgesNumber() const{
    return edges.size();
}

size_t Graph::getVertexNumber() const{
    return vertexNumber;
}


