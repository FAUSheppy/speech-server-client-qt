#include "serverconnection.h"
#include "settingkeys.h"
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QSettings>

ServerConnection::ServerConnection(QObject *parent, QSettings *settings)
{

    setAuthHeader(settings->value(SETTING_USER).toString(), settings->value(SETTING_PASS).toString());
    networkManager = new QNetworkAccessManager(parent);
    mySettings = settings;
    this->parent = parent;
}

QString ServerConnection::buildURLFromLocation(QVariant location){
    return buildURLFromLocation(location.toString());
}

QString ServerConnection::buildURLFromLocation(QString location){
    QString proto = mySettings->value(SETTING_PROTO).toString();
    QString host = mySettings->value(SETTING_HOST).toString();
    QString port = mySettings->value(SETTING_PORT).toString();
    if(!proto.endsWith("://")){
        proto += "://";
    }
    if(!port.startsWith(":")){
        port = ":" + port;
    }
    QString url = proto + host + port + location;
    return url;
}

void ServerConnection::setAuthHeader(const QString username, const QString password){

    /* prepare auth */
    QString concatenated = username + ":" + password;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString authHeaderStr = "Basic " + data;
    authHeaderData = authHeaderStr.toLocal8Bit();

}

void ServerConnection::queryStatusAll(){

    QString statusRequestUrl = buildURLFromLocation(mySettings->value(SETTING_LOC_STATE));
    QUrl trackingUrl = QUrl(statusRequestUrl);
    QNetworkRequest request(trackingUrl);
    request.setRawHeader("Authorization", authHeaderData);
    networkManager->get(request);
    //qDebug("Status query sent");
}

void ServerConnection::queryTransscript(QString trackingId){
    QString url = buildURLFromLocation(mySettings->value(SETTING_LOC_TRANSCRIPT));
    QString paramUrl = QString("%1?id=%2").arg(url, trackingId);
    QUrl transcriptUrl = QUrl(paramUrl);
    QNetworkRequest request(transcriptUrl);
    request.setRawHeader("Authorization", authHeaderData);
    networkManager->get(request);
}

void ServerConnection::submitFile(QJsonDocument jsonDocument){

    /* prepare request */
    QUrl serviceUrl = QUrl(buildURLFromLocation(mySettings->value(SETTING_LOC_SUBMIT)));
    QNetworkRequest request(serviceUrl);
    request.setRawHeader("Authorization", authHeaderData);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    /* make request */
    networkManager->post(request, jsonDocument.toJson());

    qDebug("Request submitted");
}

QNetworkAccessManager *ServerConnection::getNetworkManager(){
    return networkManager;
}
