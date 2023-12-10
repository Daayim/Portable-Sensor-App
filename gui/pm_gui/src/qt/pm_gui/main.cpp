/***************************************************************************
main.cpp:  Marine Mammal Detection Power Module application main function

Written by:
    Sandra Mercer, P.Eng.
    Engineer / Software Developer
    eSonar Inc.

Date:
    November 2021
****************************************************************************/


#include <QApplication>
#include <QFile>
#include <QScreen>
#include <QStyle>
#include <QWindow>
#include "pm_gui.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/pm_gui.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    file.close();
    a.setStyleSheet(styleSheet);

    Widget w;
    w.show();

    QScreen *screen = (w.windowHandle())->screen();
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w.size(), screen->availableGeometry()));

    return a.exec();

}   // End of main


