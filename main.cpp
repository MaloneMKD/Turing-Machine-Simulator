#include "turingmachinewindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication::setDesktopSettingsAware(false);
    QApplication a(argc, argv);
    TuringMachineWindow w;
    w.showMaximized();
    return a.exec();
}
