#include "FanControlUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FanControlUI w;
    w.show();
    return a.exec();
}
