#ifndef ABOUT_H
#define ABOUT_H

#include <QMainWindow>

class About : public QMainWindow
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    ~About();

private:
    QString* getCurrentVersion();
    QString *getServerVersion();
};

#endif // ABOUT_H
