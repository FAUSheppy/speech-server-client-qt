#include "settings.h"
#include "ui_settings.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <settingkeys.h>

#define SETTINGS_WINDOW_TITLE "Konfiguration"
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
    auto ok = new QPushButton(BUTTON_TEXT_OK);
    auto cancle = new QPushButton(BUTTON_TEXT_ABORT);

    layout->addWidget(ok, configOptions->length(), 0);
    layout->addWidget(cancle, configOptions->length(), 1);

    connect(ok, SIGNAL(released()), this, SLOT(okClose()));
    connect(cancle, SIGNAL(released()), this, SLOT(cancleClose()));
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
