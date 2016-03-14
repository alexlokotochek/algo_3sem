#ifndef GRAPHGENERATOR_H
#define GRAPHGENERATOR_H
#include "graph.h"
#include <random>
#include <chrono>
#include <QPainter>

class GraphGenerator{
public:

    std::vector<Graph::Edge> getRandomGraphEdges() { return edges; }
    std::vector<QPoint> placeVertices();
    GraphGenerator(size_t graphSize);

private:

    size_t vertex;
    std::vector<Graph::Edge> edges;

};

#endif // GRAPHGENERATOR_H
