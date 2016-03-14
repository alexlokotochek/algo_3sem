#-------------------------------------------------
#
# Project created by QtCreator 2015-10-08T15:45:32
#
#-------------------------------------------------
CONFIG += c++11
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = graph
TEMPLATE = app


SOURCES += main.cpp\
    drawer.cpp \
    graphgenerator.cpp \
    graph.cpp \
    network.cpp \
    maxflow.cpp

HEADERS  += \
    drawer.h \
    graphgenerator.h \
    graph.h \
    network.h \
    maxflow.h

FORMS    += mainwindow.ui
