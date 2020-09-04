#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

#include <QNetworkAccessManager>
#include <QSettings>



class ServerConnection
{
public:
    ServerConnection(QObject *parent, QSettings *settings);
    void queryTransscript(QString trackingId);
    void submitFile(QJsonDocument json);
    QString buildURLFromLocation(QVariant location);
    QString buildURLFromLocation(QString location);
    QNetworkAccessManager *getNetworkManager();
    void queryServerVersion();
    void getPostProcessorMap();
    void submitPostProcessorChange(QJsonDocument jsonDocument);
    void submitSpeechContextPhraseChange(QJsonDocument jsonDocument);
    void getSpeechContextPhrases();
    void getUnifiedServerConfig();
public slots:
    void queryStatusAll();
private slots:
    void queryStatusAllWrapper();
private:
    QNetworkAccessManager *networkManager;
    QByteArray authHeaderData;
    QSettings *mySettings;
    void setAuthHeader(const QString username, const QString password);
    QObject *parent;
};

#endif // SERVERCONNECTION_H
