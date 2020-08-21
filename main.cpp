#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("moncop");
    QCoreApplication::setApplicationVersion("0.1");
    QCoreApplication::setOrganizationName("BUSOC");
    QCoreApplication::setOrganizationDomain("busoc.be");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
