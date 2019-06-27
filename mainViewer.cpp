#include "GUI/viewerOnlyWindow.h"
#include <QApplication>

//-----------------------------------------------------------------------------
// Name   : main
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ViewerOnlyWindow w;
    w.show();

    return a.exec();
}
