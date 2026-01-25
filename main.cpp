#include "startwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StartWindow startWindow;
    startWindow.setWindowTitle("Arduino Simulator");
    startWindow.showMaximized();
    return a.exec();
}
