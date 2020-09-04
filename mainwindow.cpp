#include "about.h"
#include "mainwindow.h"
#include "notificationwidget.h"
#include "serverconfig.h"
#include "settings.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QFileDialog>
#include <QMainWindow>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTimer>
#include <QMessageBox>
#include <QPushButton>
#include <QAction>
#include <QProgressBar>
#include <QProcess>
#include <QSettings>
#include <QtNetwork/QNetworkAccessManager>
#include <QStandardPaths>
#include <QString>
#include <settingkeys.h>
#include <serverconnection.h>

#define FILENAME_COL          0
#define TRACKING_ID_COL       1
#define PG_BAR_COL            2
#define OPEN_DIR_COL          3
#define TRANSCRIPT_STATUS_COL 4
#define NUM_OF_COLS           5

#define WINDOW_TITLE "Memo Uploader"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow){

    /* setup ui */
    ui->setupUi(this);

    /* ensure all initial settings are set */
    #ifdef Q_OS_LINUX
        this->mySettings = new QSettings();
    #endif
    #ifdef Q_OS_WIN
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        path = path + "/settings.ini";
        this->mySettings = new QSettings(path, QSettings::IniFormat);
    #endif

    handleInitialSettings();

    serverConnection = new ServerConnection(this, mySettings);

    /* setup buttons */
    button = ui->centralWidget->findChild<QPushButton*>("pushButton");
    connect(button, SIGNAL (released()), this, SLOT (importFile()));

    /* table ui */
    tw = ui->centralWidget->findChild<QTableWidget*>("tableWidget");

    tw->verticalHeader()->setVisible(false);
    tw->horizontalHeader()->setVisible(false);
    tw->setRowCount(0);
    /* FILE | trackingId | STATUS | open dir | download completed? true/false | */
    tw->setColumnCount(NUM_OF_COLS);
    tw->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tw->setColumnHidden(TRANSCRIPT_STATUS_COL, true);

    /* table header */
    tw->insertRow(0);
    tw->setItem(0, FILENAME_COL,    new QTableWidgetItem("Dateiname"));
    tw->setItem(0, TRACKING_ID_COL, new QTableWidgetItem("Tracking-Id"));
    tw->setItem(0, PG_BAR_COL,      new QTableWidgetItem("Status"));
    tw->setItem(0, OPEN_DIR_COL,    new QTableWidgetItem("Ordner Öffnen"));
    tw->setItem(0, OPEN_DIR_COL,    new QTableWidgetItem("Ordner Öffnen"));

    /* create status update timer */
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(queryStatusAllWrapper()));
    timer->start(1000);

    /* add handler for menu configuration */
    ui->menuKonfiguration->addAction("Spracherkennung", this, SLOT(openSpeechConfigWindow()) );
    ui->menuKonfiguration->addAction("Konfiguration", this, SLOT(openConfigurationWindow()) );
    ui->menuKonfiguration->addAction("Über diese Software", this, SLOT(openAboutWindow()) );

    /* set window options */
    this->setWindowTitle(WINDOW_TITLE);
}

void MainWindow::openSpeechConfigWindow(){
    ServerConfig *serverConfig = new ServerConfig(this, mySettings);
    serverConfig->setAttribute(Qt::WA_DeleteOnClose);
    serverConfig->show();
}

void MainWindow::openConfigurationWindow(){
    Settings *settingsWindow = new Settings();
    settingsWindow->selectSettings(this->mySettings);
    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(settingsWindow, SIGNAL(destroyed()), this, SLOT(appyConfigChanges()));
    settingsWindow->show();
}

void MainWindow::openAboutWindow(){
    About *aboutWindow = new About(this, mySettings);
    aboutWindow->setAttribute(Qt::WA_DeleteOnClose);
    aboutWindow->show();
}

void MainWindow::appyConfigChanges(){
    serverConnection = new ServerConnection(this, mySettings);
}

void MainWindow::handleInitialSettings(){

    if(!mySettings->contains(SETTING_HOST)){
        mySettings->setValue(SETTING_HOST, "localhost");
    }
    if(!mySettings->contains(SETTING_PORT)){
        mySettings->setValue(SETTING_PORT, "5000");
    }
    if(!mySettings->contains(SETTING_PROTO)){
        mySettings->setValue(SETTING_PROTO, "https://");
    }
    if(!mySettings->contains(SETTING_LOC_STATE)){
        mySettings->setValue(SETTING_LOC_STATE, "/dumpstate");
    }
    if(!mySettings->contains(SETTING_LOC_SUBMIT)){
        mySettings->setValue(SETTING_LOC_SUBMIT, "/submit-async");
    }
    if(!mySettings->contains(SETTING_LOC_TRANSCRIPT)){
        mySettings->setValue(SETTING_LOC_TRANSCRIPT, "/transcript");
    }
    if(!mySettings->contains(SETTING_SAVE_DIR)){
        mySettings->setValue(SETTING_SAVE_DIR, ".");
    }
    if(!mySettings->contains(SETTING_USER)){
        mySettings->setValue(SETTING_USER, "");
    }
    if(!mySettings->contains(SETTING_PASS)){
        mySettings->setValue(SETTING_PASS, "");
    }
    if(!mySettings->contains(SETTING_LINUX_EXPLORER)){
        mySettings->setValue(SETTING_LINUX_EXPLORER, "/usr/bin/thunar");
    }
}

void MainWindow::importFile(){
    QString filename =  QFileDialog::getOpenFileName(
                this,
                "Open Document",
                QDir::currentPath(),
                "All files (*.*) ;; Document files (*.doc *.rtf);; PNG files (*.png)");

    if(filename.isNull()){
        return;
    }else{
        this->submitFileSlot(filename);
    }
}

void MainWindow::showNotification(QString str){

    auto popUp = new NotificationWidget(this);
    popUp->setPopupText(str);
    popUp->setGeometry(0, 0, popUp->width(), popUp->height());
    popUp->show();
    auto *timer = new QTimer();
    connect(timer, SIGNAL(timeout()), popUp, SLOT(fadeOut()));
    timer->start(5000);

}

void MainWindow::openContainingDir(){

    QString filePath = mySettings->value(SETTING_SAVE_DIR).toString();
    QStringList args;

    /* OS specific explorer call */
    #ifdef Q_OS_LINUX
        args << QDir::toNativeSeparators(filePath);
        QProcess::startDetached(mySettings->value(SETTING_LINUX_EXPLORER).toString(), args);
    #endif
    #ifdef Q_OS_WIN
        args << "/select," << QDir::toNativeSeparators(filePath);
        QProcess::startDetached("explorer", args);
    #endif
}

void MainWindow::updateList(QNetworkReply* reply){
    if(reply->error() != QNetworkReply::NoError){
        showNotification("HTTP Error on status Request");
        return;
    }

    /* get filename and tracking id from replay */
    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
    for(int i = 0; i<tw->rowCount(); i++){
        auto trackingId = tw->model()->data(tw->model()->index(i, TRACKING_ID_COL)).toString();

        if(json.contains(trackingId)){
            QJsonArray statusArray = json.value(trackingId).toArray();
            if(statusArray.size() == 0){
                qDebug("Status array empty wtf?");
            }

            auto currentStatus = statusArray[statusArray.size()-1].toObject();
            auto status = currentStatus["status"].toString();
            auto percentage = currentStatus["percent"].toInt();
            auto statusMsg = currentStatus["message"].toString();

            auto pg = tw->cellWidget(i, PG_BAR_COL)->findChild<QProgressBar*>();
            pg->setFormat(statusMsg);
            pg->setValue(100);

            if(percentage == -1){
                pg->setValue(100);
                pg->setStyleSheet("QProgressBar::chunk { background-color: red; }");
            }else if(percentage == 100){
                pg->setValue(percentage);
                pg->setStyleSheet("QProgressBar::chunk { background-color: green; }");
                auto status = tw->model()->data(tw->model()->index(i, TRANSCRIPT_STATUS_COL));
                auto tStatus = status.toString().toInt();
                if(tStatus == 0){
                    serverConnection->queryTransscript(trackingId);
                }
            }else{
                pg->setValue(percentage);
            }
        }
    }
}

void MainWindow::saveTranscript(QNetworkReply* reply){

    /* get return data */
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    auto transcript = json["transcript"].toString();
    auto trackingId = json["id"].toString();
    auto targetName = json["true-name"].toString();
    if(QString::compare(trackingId, "") ==  0){
        qFatal("Empty TrackingId, file might have been request by it's name.");
    }

    /* get filename for id */
    QString filename;
    int rowId = -1;
    for(int i = 0; i<tw->rowCount(); i++){
        auto curId = tw->model()->data(tw->model()->index(i, TRACKING_ID_COL)).toString();
        auto curName = tw->model()->data(tw->model()->index(i, FILENAME_COL)).toString();
        if(QString::compare(trackingId, curId) ==  0){
            filename = curName;
            rowId = i;
            break;
        }
    }

    /* save return data */
    QString fullpath = QDir(mySettings->value(SETTING_SAVE_DIR).toString()).filePath(targetName);
    QFile file(fullpath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        qWarning("Error opening File");
    }
    QTextStream out(&file);
    out.setGenerateByteOrderMark(true);
    out.setCodec("UTF-8");
    out << transcript;
    file.close();

    /* mark as downloaded */
    auto ts = new QTableWidgetItem();
    ts->setData(Qt::DisplayRole, 1);
    tw->setItem(rowId, TRANSCRIPT_STATUS_COL, ts);
}

void MainWindow::addTrackingToList(QNetworkReply* reply){
    if(reply->error() != QNetworkReply::NoError){
        showNotification("Fehler bei Dateiübertragung, keine Tracking-ID zugewiesen!");
        return;
    }

    /* add new row */
    tw->insertRow(tw->rowCount());
    auto row = tw->rowCount() - 1;

    /* get filename and tracking id from replay */
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    auto filename = json["filename"].toString();
    auto trackingId = json["trackingId"].toString();

    /* create cusomt widget for pg bar */
    auto *pgBarLayout = new QGridLayout();
    auto *pg = new QProgressBar();
    auto *pgBarCellContent = new QWidget();
    pg->setRange(0,100);
    pg->setValue(0);
    pg->setTextVisible(true);
    pg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    const QFont *font = new QFont("Times", 12, QFont::Weight::Bold, false);
    pg->setFont(*font);
    pg->setAlignment(Qt::AlignCenter);

    pgBarLayout->addWidget(pg);
    pgBarLayout->setContentsMargins(0,0,0,0);
    pgBarCellContent->setLayout(pgBarLayout);

    /* create cusomt widget for open-dir button */
    auto *openDirLayout = new QGridLayout();
    auto dirButton = new QPushButton("Ordner");
    connect(dirButton, SIGNAL (released()), this, SLOT (openContainingDir()));

    auto *openDirCellContent = new QWidget();
    openDirLayout->addWidget(dirButton);
    dirButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    openDirLayout->setContentsMargins(0,0,0,0);
    openDirCellContent->setLayout(openDirLayout);

    /* set values */
    tw->setItem(row, FILENAME_COL, new QTableWidgetItem(filename));
    tw->setItem(row, TRACKING_ID_COL, new QTableWidgetItem(trackingId));
    tw->setCellWidget(row, PG_BAR_COL, pgBarCellContent);
    tw->setCellWidget(row, OPEN_DIR_COL, openDirCellContent);


    /* set transcript status */
    auto ts = new QTableWidgetItem();
    ts->setData(Qt::DisplayRole, 0);
    tw->setItem(row, TRANSCRIPT_STATUS_COL, ts);
    tw->setColumnHidden(TRACKING_ID_COL, true);

    qDebug("Reply added to be tracked by list");
}

void MainWindow::submitFileSlot(QString filename){

    /* read audio as base64 */
    QFile sourceFile(filename);
    sourceFile.open(QIODevice::ReadOnly);
    QByteArray base64Encoded = sourceFile.readAll().toBase64();
    QString dataString = QString::fromUtf8(base64Encoded);
    sourceFile.close();

    /* prepare json */
    QJsonObject json = QJsonObject();
    QJsonValue dataFieldContent = QJsonValue(dataString);
    QFileInfo info(filename);
    QString basename(info.fileName());
    json["filename"] = filename;
    json["data"] = dataFieldContent;

    /* make request */
    connect(serverConnection->getNetworkManager(), SIGNAL(finished(QNetworkReply*)), this,
            SLOT(requestFinished(QNetworkReply*)), Qt::UniqueConnection);
    serverConnection->submitFile(QJsonDocument(json));

    qDebug("Request submission requested");
}

void MainWindow::requestFinished(QNetworkReply *reply){

    QString submitUrl = serverConnection->buildURLFromLocation(mySettings->value(SETTING_LOC_SUBMIT));
    QString statusRequestUrl = serverConnection->buildURLFromLocation(mySettings->value(SETTING_LOC_STATE));
    QString requestTranscriptUrl = serverConnection->buildURLFromLocation(mySettings->value(SETTING_LOC_TRANSCRIPT));

    if(QString::compare(reply->url().toString(), submitUrl) == 0){
        addTrackingToList(reply);
    }else if (QString::compare(reply->url().toString(), statusRequestUrl) == 0) {
        updateList(reply);
    }else if (reply->url().toString().startsWith(requestTranscriptUrl)) {
        qDebug("Saving transcript");
        saveTranscript(reply);
    }else{
        qDebug("URL-Response: %s", qUtf8Printable(reply->url().toString()));
        qFatal("Unexpected responding URL");
    }
}

void MainWindow::queryStatusAllWrapper(){
    serverConnection->queryStatusAll();
}


MainWindow::~MainWindow()
{
    delete ui;
}
