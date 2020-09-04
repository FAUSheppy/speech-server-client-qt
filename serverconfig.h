#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

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
private:
    QGridLayout* mainLayout;
    QSettings* mySettings;
    QWidget *loatListItemUiForm();
    QTableWidget* ppTable;
    QTableWidget* contextTable;
};

#endif // SERVERCONFIG_H
