#include "about.h"
#include "ui_about.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

About::About(QWidget *parent) : QMainWindow(parent) {

    QGridLayout *mainLayout = new QGridLayout();

    /* config options layout */
    QGridLayout *layoutLegal = new QGridLayout();
    QGridLayout *layoutSoftwareinfo = new QGridLayout();
    
    /* create legal box */
    QGroupBox *legalGroup = new QGroupBox();


    /* create software infor group */
    QGroupBox *softwareInfoGroup = new QGroupBox();
    QString *version = getCurrentVersion();
    QLabel *richText = new QLabel();
    richText->setText(*version);
    softwareInfoGroup->setTitle("Software Information");

    layoutSoftwareinfo->addWidget(richText);
    softwareInfoGroup->setLayout(layoutSoftwareinfo);

    /* add groups to main layout */
    mainLayout->addWidget(softwareInfoGroup);

    /* setup main window */
    QWidget *mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);
}

About::~About()
{
}

QString* About::getCurrentVersion(){
    return new QString("<p>LOLOLOL</p>");
}

QString* About::getServerVersion(){
    return new QString("<p>LOLOLOL</p>");
}
