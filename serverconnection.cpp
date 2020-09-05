#include "serverconnection.h"
#include "settingkeys.h"
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QSettings>
#include <urls.h>

#define AUTH_HEADER_NAME "Authorization"
#define MIME_JSON "application/json"

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

QString ServerConnection::buildURLFromLocation(const char *location){
    return buildURLFromLocation(QString(location));
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
    request.setRawHeader(AUTH_HEADER_NAME, authHeaderData);
    networkManager->get(request);
    //qDebug("Status query sent");
}

void ServerConnection::queryTransscript(QString trackingId){
    QString url = buildURLFromLocation(mySettings->value(SETTING_LOC_TRANSCRIPT));
    QString paramUrl = QString("%1?id=%2").arg(url, trackingId);
    QUrl transcriptUrl = QUrl(paramUrl);
    QNetworkRequest request(transcriptUrl);
    request.setRawHeader(AUTH_HEADER_NAME, authHeaderData);
    networkManager->get(request);
}

void ServerConnection::submitFile(QJsonDocument jsonDocument){

    /* prepare request */
    QUrl serviceUrl = QUrl(buildURLFromLocation(mySettings->value(SETTING_LOC_SUBMIT)));
    QNetworkRequest request(serviceUrl);
    request.setRawHeader(AUTH_HEADER_NAME, authHeaderData);
    request.setHeader(QNetworkRequest::ContentTypeHeader, MIME_JSON);

    /* make request */
    networkManager->post(request, jsonDocument.toJson());

    qDebug("Request submitted");
}

void ServerConnection::queryServerVersion(){
    QUrl serviceUrl = QUrl(buildURLFromLocation(QString(SERVER_INFO)));
    QNetworkRequest request(serviceUrl);
    request.setRawHeader(AUTH_HEADER_NAME, authHeaderData);
    networkManager->get(request);
}

QNetworkAccessManager *ServerConnection::getNetworkManager(){
    return networkManager;
}

void ServerConnection::submitPostProcessorChange(QString keyword, QString repl, bool remove){

    QJsonObject json = QJsonObject();
    json["keyword-regex"] = keyword;
    json["replacement"] = repl;
    if(remove){
        json["action"] = "delete";
    }else{
        json["action"] = "add";
    }

    QUrl serviceUrl = QUrl(buildURLFromLocation(QString(PP_EDIT)));
    QNetworkRequest request(serviceUrl);
    request.setRawHeader(AUTH_HEADER_NAME, authHeaderData);
    request.setHeader(QNetworkRequest::ContentTypeHeader, MIME_JSON);
    networkManager->post(request, QJsonDocument(json).toJson());
}

void ServerConnection::submitSpeechContextPhraseChange(QString phrase, bool remove){

    QJsonObject json = QJsonObject();
    json["phrase"] = phrase;
    if(remove){
        json["action"] = "delete";
    }else{
        json["action"] = "add";
    }

    QUrl serviceUrl = QUrl(buildURLFromLocation(QString(CONTEXT_EDIT)));
    QNetworkRequest request(serviceUrl);
    request.setRawHeader(AUTH_HEADER_NAME, authHeaderData);
    request.setHeader(QNetworkRequest::ContentTypeHeader, MIME_JSON);
    networkManager->post(request, QJsonDocument(json).toJson());
}

void ServerConnection::getSpeechContextPhrases(){
    QUrl serviceUrl = QUrl(buildURLFromLocation(QString(CONTEXT_GET)));
    QNetworkRequest request(serviceUrl);
    request.setRawHeader(AUTH_HEADER_NAME, authHeaderData);
    networkManager->get(request);
}

void ServerConnection::getPostProcessorMap(){
    QUrl serviceUrl = QUrl(buildURLFromLocation(QString(PP_GET)));
    QNetworkRequest request(serviceUrl);
    request.setRawHeader(AUTH_HEADER_NAME, authHeaderData);
    networkManager->get(request);
}

void ServerConnection::getUnifiedServerConfig(){
    QUrl serviceUrl = QUrl(buildURLFromLocation(QString(UNIFIED_GET)));
    QNetworkRequest request(serviceUrl);
    request.setRawHeader(AUTH_HEADER_NAME, authHeaderData);
    networkManager->get(request);
}
