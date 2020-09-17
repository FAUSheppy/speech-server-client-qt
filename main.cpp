#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    a.setApplicationName("Speech-To-Text");
    a.setOrganizationName("Lantia IT");
    a.setOrganizationDomain("lantia-it.de");

    return a.exec();
}
