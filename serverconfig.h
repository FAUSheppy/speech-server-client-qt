#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include "serverconnection.h"

#include <QGridLayout>
#include <QMainWindow>
#include <QNetworkReply>
#include <QSettings>
#include <QTableWidget>
#include <QWidget>



class ServerConfig : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerConfig(QWidget *parent = nullptr, QSettings *settings = nullptr);
    virtual ~ServerConfig();
private slots:
    void finishedRequest(QNetworkReply*);
    void addNewPP();
    void addNewContext();
    void removePP();
    void removeContext();
private:
    QGridLayout* mainLayout;
    QSettings* mySettings;
    QWidget *loatListItemUiForm();
    QTableWidget* ppTable;
    QTableWidget* contextTable;
    ServerConnection* sc;
    void askFlushServerCache();
};

#endif // SERVERCONFIG_H
