#ifndef ABOUT_H
#define ABOUT_H

#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMainWindow>
#include <QNetworkReply>
#include <QSettings>

class About : public QMainWindow
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr, QSettings *settings = nullptr);
    ~About();

private slots:
    void handleServerVersion(QNetworkReply*);
private:
    QGridLayout* mainLayout;
    QGridLayout* layoutSoftwareinfo;
    QLabel* serverInfo;
    QGroupBox* softwareInfoGroup;
    QSettings* mySettings;
    QString* getCurrentVersion();
    QString* getServerVersion();
};

#endif // ABOUT_H
