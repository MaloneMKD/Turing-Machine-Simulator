#include "turingmachinewindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TuringMachineWindow w;
    w.showFullScreen();
    return a.exec();
}
