#include "drawer.h"
#include "graphgenerator.h"
#include "graph.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>
#include <QPolygon>
#include <QTime>
#include <QCoreApplication>
#include <QVector2D>
#include <QTimer>
#include "graphgenerator.h"
#include "maxflow.h"
#include "network.h"
#include <string>
using namespace std;

Drawer::Drawer(QWidget *parent) : QWidget(parent){
    setScreenSizeInfo();
    Graph g = generateGraph();
    runAlgo(g);
    setLevels();

    this->timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(reshape()));
    this->timer->start(timerConst);
}

void Drawer::setLevels(){
    maxLevel = points.size()*2;
    double yLineStep = (screen_height*0.9-screen_height*0.1)/maxLevel;
    xLineStep = (screen_width*0.95 - screen_width*0.55)/points.size();
    QPoint a(screen_width*0.55, screen_height*0.9),
           b(screen_width*0.95, screen_height*0.9);
    for (int i = 0; i < maxLevel; ++i){
        QLine levelLine(QPoint(a.x(), a.y()-i*yLineStep),
                        QPoint(b.x(), b.y()-i*yLineStep));
        levelLines.push_back(levelLine);
    }
    for (size_t i = 0; i < points.size(); ++i){
        QPoint levelPoint(a.x() + (i+0.5)*xLineStep, a.y());
        pointsOnLevels.push_back(levelPoint);
    }
}

Graph Drawer::generateGraph(){
    GraphGenerator gg(graphSize);
    vector<Graph::Edge> primaryEdges = gg.getRandomGraphEdges();
    Network::NetEdge edge;
    Graph::Edge primaryEdge;
    for (size_t i = 0; i < primaryEdges.size(); ++i){
        primaryEdge = primaryEdges[i];
        edge = { primaryEdge.from, primaryEdge.to, primaryEdge.weight, 0 };
        edges.push_back(edge);
        edge = { primaryEdge.to, primaryEdge.from, 0, 0 };
        edges.push_back(edge); // backedge
    }
    points = gg.placeVertices();
    height.assign(graphSize, 0);
    excess.assign(graphSize, 0);
    Graph g;
    g.setGraphInfo(graphSize);
    g.setEdges(primaryEdges);
    changedEdgePosition = -1;
    changedPointPosition = -1;
    return g;
}

void Drawer::runAlgo(Graph g){
    MaxFlow mf(g);
    maxFlowValueString = "MAXFLOW : ";
    maxFlowValueString.append(QString::number(mf.findMaxFlow()));
    steps = mf.getSteps();
    stepsIterator = 0;
    sprintf(stepDescription, "BEGIN");
}

void Drawer::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
    switch (event->key()){
        case Qt::Key_Space: {stopTimer(); break;}
        case Qt::Key_W: {doNextStep(false, true, true); break;}// w=next
        case Qt::Key_Q: {doPrevStep(); break;}// q=prev
        default: {};
    }
}

void Drawer::stopTimer(){
    this->timer->stop();
}

void Drawer::setScreenSizeInfo(){
    QRect rec = QApplication::desktop()->screenGeometry();
    screen_height = rec.height();
    screen_width = rec.width();
    setWindowState(Qt::WindowFullScreen);
    QRect screen = QApplication::desktop()->screenGeometry();
    resize(screen.width(), screen.height());
}


void Drawer::writeFlowMessage(MaxFlow::Change step){
    sprintf(stepDescription,
            "PUSHED %d from %zu to %zu",
            step.delta,
            edges[step.who].from,
            edges[step.who].to);
}

void Drawer::writeHeightMessage(MaxFlow::Change step){
    sprintf(stepDescription,
            "RELABEL %zu: old = %d, new = %d",
            step.who,
            height[step.who],
            height[step.who]-step.delta);
}

void Drawer::writeExcessMessage(MaxFlow::Change step){
    if (step.who == 0){
        writeExcessFromSourceMessage(step.delta);
    }else{
        sprintf(stepDescription, "EXCESS %zu: old = %d, new = %d",
                step.who,
                excess[step.who],
                excess[step.who]-step.delta);
    }
}

void Drawer::writeExcessFromSourceMessage(int delta){
    if (delta>0)
        sprintf(stepDescription, "EXCESS from source decreased");
    else
        sprintf(stepDescription, "EXCESS from source increased");
}

void Drawer::flowStep(MaxFlow::Change step, bool save, bool print, int delta){
    if (save)
        edges[step.who].flow += delta;
    changedEdgePosition = step.who;
    changedPointPosition = -1;
    if (print)
        writeFlowMessage(step);
}

void Drawer::heightStep(MaxFlow::Change step, bool save, bool print, int delta){
    if (save)
        height[step.who] += delta;
    changedEdgePosition = -1;
    changedPointPosition = step.who;
    if (print)
        writeHeightMessage(step);
}

void Drawer::excessStep(MaxFlow::Change step, bool save, bool print, int delta){
    if (save)
        excess[step.who] += delta;
    changedEdgePosition = -1;
    changedPointPosition = step.who;
    if (print)
        writeExcessMessage(step);
}

void Drawer::doNextStep(bool previous, bool save, bool print){
    if (stepsIterator < steps.size()-1){
        stepsIterator++;
        MaxFlow::Change step = steps[stepsIterator];
        int delta = step.delta;
        if (previous)
            delta *= (-1);
        switch(step.what){
            case END: {
                excess[0] = 0;
                sprintf(stepDescription, "END");
                break;
            }
            case FLOW: {
                flowStep(step, save, print, delta);
                break;
            }
            case HEIGHT: {
                heightStep(step, save, print, delta);
                break;
            }
            case EXCESS: {
                excessStep(step, save, print, delta);
                break;
            }
            default : {}
        }
        if (print)
            update();
    }
}

void Drawer::doPrevStep(){
    // значение stepsIterator - какой шаг был выполнен последним
    // делаем 2 шага назад и 1 вперед (чтобы вывести, что делал предпоследний)
    if (stepsIterator > 1){
        stepsIterator--;
        doNextStep(true, true, false);
        stepsIterator-=2;
        doNextStep(false, false, true);
    }else{
        //BEGIN
        sprintf(stepDescription, "BEGIN");
    }
}

double Drawer::Drawer::kulon(double x, const double k) const{
    return k/(x*x);
}

double Drawer::guk(double x, const double k) const{
    return k*log(x/springLength);
}

void Drawer::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing,
                          QPainter::TextAntialiasing);
    painter.setBrush(Qt::white);
    painter.drawRect(rect());
    painter.setBrush(Qt::black);
    drawChangedPoint(painter);
    drawEdges(painter, pointsOnLevels, true);
    drawEdges(painter, points, false);

    drawPoints(painter);
    printText(painter);
    drawLevels(painter);
}

void Drawer::drawChangedPoint(QPainter &painter){
    if (changedPointPosition != -1){
        painter.setBrush(QColor("red"));
        painter.drawEllipse(points[changedPointPosition], 25, 25);
        painter.setBrush(Qt::black);
    }
}

void Drawer::drawPoints(QPainter &painter){
    painter.setBrush(QColor("light green"));
    char toWrite[100];
    for (size_t i = 1; i < points.size()-1; ++i) {
        painter.drawEllipse(points[i], 15, 15);
        sprintf(toWrite, "%d", excess[i]);
        painter.drawText(QPoint(points[i].x()-4, points[i].y()+4), toWrite);
    }
    painter.setBrush(QColor("light blue"));
    painter.drawEllipse(points[0], 15, 15);
    painter.setBrush(QColor("pink"));
    painter.drawEllipse(points[points.size()-1], 15, 15);

    sprintf(toWrite, "S");
    painter.drawText(QPoint(points[0].x()-4, points[0].y()+4), toWrite);
    sprintf(toWrite, "%d", height[points.size()-1]);
    painter.drawText(QPoint(points[points.size()-1].x()-4, points[points.size()-1].y()+4), toWrite);

}

void Drawer::printText(QPainter &painter){
    painter.setBrush(Qt::black);
    QPen textPen(QColor("dark green"));
    painter.save();
    painter.setFont(QFont("Helvetica", 30, QFont::Bold));
    painter.setPen(textPen);
    painter.drawText(QPoint(screen_width*0.1, screen_height*0.8),
                     stepDescription);
    painter.restore();
    painter.drawText(QPoint(screen_width*0.1, screen_height*0.8+30),
                     maxFlowValueString);
    painter.drawText(QPoint(screen_width*0.1, screen_height*0.8+60),
                     "Yellow edge color: pushed through edge");
    painter.drawText(QPoint(screen_width*0.1, screen_height*0.8+75),
                     "Pink edge color: pushed through backedge");
    painter.drawText(QPoint(screen_width*0.1, screen_height*0.8+90),
                     "Q : previous | W : next | SPACE : stop moving");
}

void Drawer::reshape(){
    vector<QVector2D> force(points.size());
    double stepsPerClock = 1;
    while(stepsPerClock>0){
        for (size_t v = 0; v < points.size(); ++v){
            force[v] = QVector2D(0, 0);
            for (size_t u = 0; u < points.size(); ++u){
                if (u != v){
                    QVector2D delta(points[v]-points[u]);
                    force[v] += delta.normalized()*kulon(delta.length(), kulonK);
                }
            }
        }
        for (size_t e = 0; e < edges.size(); ++e){
            int v = edges[e].from, u = edges[e].to;
            QVector2D delta(points[u]-points[v]);
            force[u] += (-1)*delta.normalized()*guk(delta.length(), gukC);
            force[v] += delta.normalized()*guk(delta.length(), gukC);
        }
        for (size_t v = 0; v < points.size(); ++v){
            points[v] += (force[v].normalized()*min(force[v].length()*1.0,
                          stepsPerClock)).toPoint();
            sideAction(v);
        }
        stepsPerClock--;
    }
    update();
}

void Drawer::sideAction(int v){
    QVector2D fromLeft(points[v]-QPoint(screen_width*0.05, points[v].y())),
              fromRight(points[v]-QPoint(screen_width*0.5, points[v].y())),
              fromUp(points[v]-QPoint(points[v].x(), screen_height*0.05)),
              fromDown(points[v]-QPoint(points[v].x(), screen_height*0.75));
    points[v] += (fromLeft.normalized()*kulon(fromLeft.length(), sideC)).toPoint();
    points[v] += (fromRight.normalized()*kulon(fromRight.length(), sideC)).toPoint();
    points[v] += (fromDown.normalized()*kulon(fromDown.length(), sideC)).toPoint();
    points[v] += (fromUp.normalized()*kulon(fromUp.length(), sideC)).toPoint();
}

void Drawer::drawEdges(QPainter &painter,
                       vector<QPoint> &vertices,
                       bool levels) {
    QPen highlightEdgePen(QColor("yellow"), 8);
    QPen highlightBackEdgePen(QColor("pink"), 8);
    painter.save();
    QString toWrite;
    for (int i = 0; i < (int)edges.size(); ++i) {
        toWrite = "";
        if (levels && edges[i].flow==0)
            continue;
        QPoint p0 = vertices[edges[i].from];
        QPoint p1 = vertices[edges[i].to];
        QLineF line(p0, p1);
        if (i%2==0){
            if (i == changedEdgePosition){
                painter.setPen(highlightEdgePen);
                painter.drawLine(line);
                painter.restore();
            }
            toWrite.append(QString::number(edges[i].flow));
            toWrite.append("/");
            toWrite.append(QString::number(edges[i].capacity));
            if (!levels)
                painter.drawText(p0+ (p1-p0)/2, toWrite);
            drawArrow(painter, p0, p1);
        }
        else{
            if (i == changedEdgePosition){
                painter.setPen(highlightBackEdgePen);
                painter.drawLine(line);
                painter.restore();
                toWrite.append(QString::number(edges[i-1].flow));
                toWrite.append(QString::number(edges[i-1].capacity));
                // back edge = edge+1
                painter.drawText(p0+ (p1-p0)/2, toWrite);
            }
        }
        painter.drawLine(line);
    }
}

void Drawer::drawArrow(QPainter &painter, QPoint p0, QPoint p1){
    QPolygon arrow;
    QLineF line(p0, p1);
    arrow << QPoint(0, -3)
          << QPoint(40, 0)
          << QPoint(0, 3);
    double dx = p1.x() - p0.x();
    double dy = p1.y() - p0.y();
    double angle = atan2(dy, dx);
    QPointF p2 (p1.x() - 40*cos(angle),
                p1.y() - 40*sin(angle));
    painter.save();
    painter.translate(p2);
    painter.rotate(-line.angle());
    painter.drawConvexPolygon(arrow);
    painter.restore();
}

void Drawer::drawLevels(QPainter &painter){
    for (int i = 0; i < maxLevel; ++i){
        painter.drawLine(levelLines[i]);
        for (size_t j = 0; j < points.size(); ++j){
            if (height[j]==i){
                pointsOnLevels[j] = QPoint(levelLines[i].x1()+(j+0.5)*xLineStep, levelLines[i].y1());
                if (j == 0){
                    painter.setBrush(QColor("light blue"));
                    painter.drawEllipse(pointsOnLevels[j], 20, 20);
                    painter.drawText(QPoint(pointsOnLevels[j].x()-4, pointsOnLevels[j].y()+4), "S");
                }
                else if (j == points.size()-1){
                    painter.setBrush(QColor("pink"));
                    painter.drawEllipse(pointsOnLevels[j], 20, 20);
                    painter.drawText(QPoint(pointsOnLevels[j].x()-4, pointsOnLevels[j].y()+4), "T");
                }
                else{
                    painter.setBrush(QColor("light green"));
                    painter.drawEllipse(pointsOnLevels[j], 20, 20);
                }

            }
        }
    }
    painter.setBrush(Qt::black);
}
