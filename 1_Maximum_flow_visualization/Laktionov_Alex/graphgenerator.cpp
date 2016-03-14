#include "graphgenerator.h"
#include "graph.h"
#include <vector>
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>
#include <chrono>

using namespace std;
GraphGenerator::GraphGenerator(size_t graphSize){
    Graph::Edge edge;
    vertex = graphSize;
    for (size_t i = 0; i < vertex-1; ++i){
        for (size_t j = i+1; j < vertex; ++j){
            if (rand()%4 != 0){
                edge.from = i;
                edge.to = j;
                edge.weight = rand()%9+1;
                edges.push_back(edge);
            }
        }
    }
}

std::vector<QPoint> GraphGenerator::placeVertices()
{
    QRect rec = QApplication::desktop()->screenGeometry();
    int screen_height = rec.height();
    int screen_width = rec.width();
    std::vector<QPoint> points;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distributionX(screen_width*0.1, screen_width*0.45);
    std::uniform_int_distribution<int> distributionY(screen_height*0.1, screen_height*0.7);
    for (size_t i = 0; i < vertex; ++i) {
        QPoint p(distributionX(generator), distributionY(generator));
        points.push_back(p);
    }
    return points;

}
