#include "GUI/dblfDebugWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DBLFDebugWindow w;
    w.show();

    return a.exec();
}
