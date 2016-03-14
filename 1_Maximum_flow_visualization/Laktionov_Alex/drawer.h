#ifndef DRAWER_H
#define DRAWER_H

#include <QWidget>
#include "graph.h"
#include "network.h"
#include "graphgenerator.h"
#include "maxflow.h"
#include <QString>

const int graphSize = 6;
const double kulonK = 2000; // отталкивание вершин
const double gukC = 30; // сжатие рёбер
const double sideC = 10; // отталкивание от сторон
const double stepsPerClock = 1; // движений за timerConst мс
const double springLength = 350; // к какой длине стремятся рёбра
const int timerConst = 1; // раз в сколько мс перерисовка

class Drawer : public QWidget{ Q_OBJECT
public:
    explicit Drawer (QWidget *parent = 0);
    enum {
        FLOW,
        HEIGHT,
        EXCESS,
        BEGIN, // нулевой
        END // и последний шаг, для удобной индексации
    };

private:

    vector<QPoint> points;
    vector<Network::NetEdge> edges;
    vector<int> height;
    vector<int> excess;
    QString maxFlowValueString;

    int screen_height;
    int screen_width;


    vector<QPoint> pointsOnLevels;
    vector<QLine> levelLines;
    int maxLevel;
    double xLineStep;
    void setLevels();

    QTimer *timer;

    void drawEdges(QPainter &painter,
                   vector<QPoint> &vertices,
                   bool levels);
    void drawLevels(QPainter &painter);

    double kulon(double x, double k) const;
    double guk(double x, double k) const;
    void sideAction(int v);

    void writeExcessFromSourceMessage(int delta);
    void writeExcessMessage(MaxFlow::Change step);
    void writeHeightMessage(MaxFlow::Change step);
    void writeFlowMessage(MaxFlow::Change step);
    void setScreenSizeInfo();
    Graph generateGraph();
    void runAlgo(Graph g);

    vector<MaxFlow::Change> steps;
    size_t stepsIterator;
    char stepDescription[2<<10];
    void flowStep(MaxFlow::Change step, bool save, bool print, int delta);
    void heightStep(MaxFlow::Change step, bool save, bool print, int delta);
    void excessStep(MaxFlow::Change step, bool save, bool print, int delta);

    int changedEdgePosition;
    int changedPointPosition;

    void printText(QPainter &painter);
    void drawChangedPoint(QPainter &painter);
    void drawPoints(QPainter &painter);
    void drawArrow(QPainter &painter, QPoint p0, QPoint p1);

signals:
    void stopDrawing();
    void nextStep();
    void prevStep();

public slots:
    void reshape();
    void stopTimer();
    void doNextStep(bool cancel,
                    bool save,
                    bool description);
    void doPrevStep();

protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *event);

};
#endif // DRAWER_H
