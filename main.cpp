#include "mainwindow.h"

#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont appFont = a.font();
    int baseSize = appFont.pointSize();
    if (baseSize <= 0) {
        baseSize = 10;
    }
    appFont.setPointSize(baseSize + 2);
    a.setFont(appFont);

    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
