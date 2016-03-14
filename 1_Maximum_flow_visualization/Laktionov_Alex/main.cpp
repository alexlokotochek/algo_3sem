#include "drawer.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Drawer d;
    d.showFullScreen();
    return a.exec();
}
