#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QFileDialog>
#include <QMainWindow>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtNetwork/QNetworkAccessManager>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    button = ui->centralWidget->findChild<QPushButton*>("pushButton");
    connect(button, SIGNAL (released()), this, SLOT (importFile()));
}


void MainWindow::importFile()
{
    QString filename =  QFileDialog::getOpenFileName(
          this,
          "Open Document",
          QDir::currentPath(),
          "All files (*.*) ;; Document files (*.doc *.rtf);; PNG files (*.png)");
    if( !filename.isNull() ){
        return;
    }
}

void MainWindow::submitFile(QString filename)
{
    /* prepare auth */
    QString username = "lol";
    QString password = "lol";
    QString concatenated = username + ":" + password;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString authHeaderData = "Basic " + data;

    /* prepare request */
    QUrl serviceUrl = QUrl("http://localhost:5000");
    QNetworkRequest request(serviceUrl);
    request.setRawHeader("Authorization", authHeaderData.toLocal8Bit());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    /* read audio as base64 */
    QFile sourceFile(filename);
    QByteArray base64Encoded = sourceFile.readAll().toBase64();
    QString dataString = QString::fromUtf8(base64Encoded);

    /* prepare json */
    QJsonObject json = QJsonObject();
    QJsonValue dataFieldContent = QJsonValue(dataString);

    QFileInfo info(filename);
    QString basename(info.fileName());
    json["filename"] = filename;
    json["data"] = dataFieldContent;

    /* make request */
    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this,
                    SLOT(fileSubmissionFinished(QNetworkReply*)));
    networkManager->post(request, QJsonDocument(json).toJson());
}



MainWindow::~MainWindow()
{
    delete ui;
}
