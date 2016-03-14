#ifndef ____Graph__
#define ____Graph__

#include <vector>
#include <iostream>
using namespace std;

class Graph {
public:

    struct Edge {
        size_t from;
        size_t to;
        int weight;
    };    

    Edge getEdge(size_t index) const;

    size_t getEdgesNumber() const ;

    size_t getVertexNumber() const;

    void setGraphInfo(size_t inp_vertexNumber);

    void setEdges (vector<Edge> input);

    void setGraph(size_t vertexNumber_input, vector<Edge> edges_input);

private:

    size_t vertexNumber;
    vector<Edge> edges;

};
#endif /* defined(____Graph__) */
