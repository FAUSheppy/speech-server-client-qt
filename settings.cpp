#include "serverconnection.h"
#include "settings.h"
#include "ui_settings.h"

#include <QDialog>
#include <QFile>
#include <QFileInfo>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkReply>
#include <QPushButton>
#include <QSettings>
#include <settingkeys.h>

#define SETTINGS_WINDOW_TITLE "Konfiguration"
#define BUTTON_TEXT_CHECK "Konfiguration Testen"
#define BUTTON_TEXT_ABORT "Abbrechen"
#define BUTTON_TEXT_OK    "OK"

Settings::Settings(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
    configLineEditMap = new QHash<QString, QLineEdit*>();

    /* config options */
    configOptions = new QStringList();
    configOptions->append("Host");
    configOptions->append("Port");
    configOptions->append("Protokoll");
    configOptions->append("Transcript Ordner");
    configOptions->append("Username");
    configOptions->append("Passwort");

    #ifdef Q_OS_LINUX
        configOptions->append("Linux Explorer");
    #endif

    /* config option keys */
    configOptionsKeys = new QStringList();
    configOptionsKeys->append(SETTING_HOST);
    configOptionsKeys->append(SETTING_PORT);
    configOptionsKeys->append(SETTING_PROTO);
    configOptionsKeys->append(SETTING_SAVE_DIR);
    configOptionsKeys->append(SETTING_USER);
    configOptionsKeys->append(SETTING_PASS);
    #ifdef Q_OS_LINUX
        configOptionsKeys->append(SETTING_LINUX_EXPLORER);
    #endif

    this->setWindowTitle(SETTINGS_WINDOW_TITLE);
}

void Settings::selectSettings(QSettings *selectedSettings){
    this->mySettings = selectedSettings;

    /* config options layout */
    auto cw = this->findChild<QWidget*>("centralwidget");
    QGridLayout *layout = static_cast<QGridLayout*>(cw->layout());

    for(int i = 0; i < configOptions->length(); i++){

        /* create widgets */
        QLabel *label = new QLabel(this);
        label->setText(configOptions->at(i));
        QLineEdit *edit = new QLineEdit();
        edit->setText(this->mySettings->value(configOptionsKeys->at(i)).toString());

        /* add widgets to layout */
        layout->addWidget(label, i, 0);
        layout->addWidget(edit, i, 1);

        /* set stretch factors */
        layout->setColumnStretch(0, 1);
        layout->setColumnStretch(1, 1);

        /* remeber options for laters save */
        configLineEditMap->insert(configOptionsKeys->at(i), edit);
    }

    /* buttons */
    auto check = new QPushButton(BUTTON_TEXT_CHECK);
    auto ok = new QPushButton(BUTTON_TEXT_OK);
    auto cancle = new QPushButton(BUTTON_TEXT_ABORT);

    layout->addWidget(check, configOptions->length(), 0);
    layout->addWidget(ok, configOptions->length()+1, 0);
    layout->addWidget(cancle, configOptions->length()+1, 1);

    connect(check, SIGNAL(released()), this, SLOT(checkConfig()));
    connect(ok, SIGNAL(released()), this, SLOT(okClose()));
    connect(cancle, SIGNAL(released()), this, SLOT(cancleClose()));
}

void Settings::checkConfig(){
    ServerConnection *sc = new ServerConnection(this, mySettings);
    connect(sc->getNetworkManager(), SIGNAL(finished(QNetworkReply*)), this, SLOT(handleTestConnectionResult(QNetworkReply*)));
    sc->queryServerVersion();
}

void Settings::handleTestConnectionResult(QNetworkReply* reply){
    QString *dialogText;
    bool error = false;

    if(reply->error() != QNetworkReply::NoError){
        dialogText = new QString(reply->errorString());
        error = true;
    }else {
        dialogText = new QString("Verbinndung Ok");
    }
    #ifdef Q_OS_LINUX
        auto *fi = new QFileInfo(mySettings->value(SETTING_LINUX_EXPLORER).toString());
        if(!fi->isExecutable()){
            dialogText->append("\nExplorer nicht ausführbar!");
            error = true;
        }
    #endif

    QLabel *testResult = new QLabel("Ok!");
    testResult->setStyleSheet("QLabel { color : green; }");
    if(error){
        QMessageBox *info = new QMessageBox();
        info->setAttribute(Qt::WA_DeleteOnClose);
        info->setWindowTitle("Konfiguration - Fehler!");
        info->setText(*dialogText);
        info->show();
        testResult = new QLabel("Konfigurationsfehler.");
        testResult->setStyleSheet("QLabel { color : red; }");
    }

    auto cw = this->findChild<QWidget*>("centralwidget");
    QGridLayout *layout = static_cast<QGridLayout*>(cw->layout());
    layout->addWidget(testResult, configOptions->length(), 1);
}

void Settings::cancleClose(){
    if(false){
        // TODO warning
    }
    this->close();
}

void Settings::okClose(){
    QSettings mySettings;
    for(auto key : configLineEditMap->keys()){
        QString input = configLineEditMap->take(key)->text();
        if(input.compare("") != 0){
            this->mySettings->setValue(key, input);
        }
    }
    this->close();
}

Settings::~Settings()
{
    delete ui;
}
