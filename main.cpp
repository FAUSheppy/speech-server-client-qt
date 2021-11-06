#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    a.setApplicationName("Speech-To-Text");
    a.setOrganizationName("Potaris IT | Yannik Schmidt");
    a.setOrganizationDomain("potaris.de");

    return a.exec();
}
