#ifndef SETTINGS_H
#define SETTINGS_H

#include "serverconnection.h"

#include <QMainWindow>
#include <QLineEdit>
#include <QSettings>
#include <QNetworkReply>
#include <QLabel>

namespace Ui {
class settings;
}

class Settings : public QMainWindow
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    void selectSettings(QSettings *selectedSettings);
private slots:
    void okClose();
    void cancleClose();
    void checkConfig();
    void handleTestConnectionResult(QNetworkReply *reply);
private:
    ServerConnection *sc;
    Ui::settings *ui;
    QSettings  *mySettings;
    QHash<QString, QLineEdit*> *configLineEditMap;
    QStringList *configOptionsKeys;
    QStringList *configOptions;
    void saveSetting();
    QLabel *currentConfigCheckDisplay;
};

#endif // SETTINGS_H
