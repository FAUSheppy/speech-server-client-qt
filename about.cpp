#include "about.h"
#include "serverconnection.h"
#include "ui_about.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QJsonDocument>
#include <QLabel>

#define Y_POS_VERSION 0
#define Y_POS_SERVER_INFO 1
#define Y_POS_BUILD_DATE 2
#define Y_POS_BUILD_HOST 3

About::About(QWidget *parent, QSettings *settings) : QMainWindow(parent) {

    mySettings = settings;
    mainLayout = new QGridLayout();

    /* create legal box */
    QGridLayout *layoutLegal = new QGridLayout();
    QGroupBox *legalGroup = new QGroupBox();
    legalGroup->setTitle("Rechtliches");
    QLabel *licenseLabel = new QLabel("Diese Software wird unter der <a href=\"https://www.gnu.org/licenses/gpl-3.0-standalone.html\">GPLv3</a> verbreitet."
                                      "<br>Code und Kompelierungsinstruktionen sind konform zur Lizenz <a href=\"https://github.com/FAUSheppy/speech-server-client-qt\">hier</a> "
                                      "zugänglich.");
    licenseLabel->setOpenExternalLinks(true);
    layoutLegal->addWidget(licenseLabel, 0, 0);
    layoutLegal->addWidget(new QLabel("Author: Yannik Schmidt"), 1, 0);
    legalGroup->setLayout(layoutLegal);

    /* create software infor group */
    layoutSoftwareinfo = new QGridLayout();
    softwareInfoGroup = new QGroupBox();
    QString *version = getCurrentVersion();
    QLabel *versionLabelIdent = new QLabel("GUI Version:");
    QLabel *versionLabel = new QLabel();
    serverInfo = new QLabel("Wird ermittelt..");
    versionLabel->setText(*version);

    QLabel *buildDateLabelIdent = new QLabel("Version gebaut:");
    QLabel *buildDateLabel = new QLabel(BUILD_DATE);
    layoutSoftwareinfo->addWidget(buildDateLabelIdent, Y_POS_BUILD_DATE, 0);
    layoutSoftwareinfo->addWidget(buildDateLabel, Y_POS_BUILD_DATE, 1);

    QLabel *buildHostLabelIdent = new QLabel("Kompilations Host:");
    QLabel *buildHostLabel = new QLabel(BUILD_HOST);
    layoutSoftwareinfo->addWidget(buildHostLabelIdent, Y_POS_BUILD_HOST, 0);
    layoutSoftwareinfo->addWidget(buildHostLabel, Y_POS_BUILD_HOST, 1);

    softwareInfoGroup->setTitle("Software Information");
    layoutSoftwareinfo->addWidget(versionLabelIdent, Y_POS_VERSION, 0);
    layoutSoftwareinfo->addWidget(versionLabel, Y_POS_VERSION, 1);
    layoutSoftwareinfo->addWidget(new QLabel("Server Version:"), Y_POS_SERVER_INFO, 0);
    layoutSoftwareinfo->addWidget(serverInfo, Y_POS_SERVER_INFO, 1);
    softwareInfoGroup->setLayout(layoutSoftwareinfo);

    /* add groups to main layout */
    mainLayout->addWidget(softwareInfoGroup);
    mainLayout->addWidget(legalGroup);

    /* setup main window */
    QWidget *mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);

    /* get server info  and set as soon as possible*/
    ServerConnection *sc = new ServerConnection(this, mySettings);
    connect(sc->getNetworkManager(), SIGNAL(finished(QNetworkReply*)), this, SLOT(handleServerVersion(QNetworkReply*)));
    sc->queryServerVersion();
}

About::~About()
{
}

void About::handleServerVersion(QNetworkReply* reply){
    if(reply->error() != QNetworkReply::NoError){
        serverInfo->setText(reply->errorString());
    }else {
        QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
        auto serverInfoValue = json["server-version"].toString();
        serverInfo->setText(serverInfoValue + " (kompatibel)");
    }
}

QString* About::getCurrentVersion(){
    QString *string = new QString(GIT_VERSION);
    return string;
}
