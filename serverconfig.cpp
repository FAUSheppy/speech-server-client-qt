#include "serverconfig.h"
#include "serverconnection.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QListWidget>
#include <QWidget>
#include <QtUiTools>

#include "multivalueinputdialog.h"
#include "urls.h"

ServerConfig::ServerConfig(QWidget *parent, QSettings *settings) : QMainWindow(parent) {

    mySettings = settings;

    sc = new ServerConnection(this, mySettings);
    connect(sc->getNetworkManager(), SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedRequest(QNetworkReply*)), Qt::UniqueConnection);
    sc->getUnifiedServerConfig();

    /* post processing group */
    QGroupBox *pp = new QGroupBox();
    pp->setTitle("Post Processing");
    ppTable = new QTableWidget();

    /* table ui */
    ppTable->verticalHeader()->setVisible(false);
    ppTable->horizontalHeader()->setVisible(false);
    ppTable->setRowCount(0);

    QStringList *headerList = new QStringList();
    headerList->append("Suchwort");
    headerList->append("Ersetzen durch");
    headerList->append("Entfernen");

    ppTable->setColumnCount(headerList->length());
    ppTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QGridLayout *innerLayoutPP = new QGridLayout();
    QPushButton *addNew = new QPushButton("Neue Regel");
    innerLayoutPP->addWidget(ppTable);
    innerLayoutPP->addWidget(addNew);
    pp->setLayout(innerLayoutPP);

    /* context phrases group */
    QGridLayout *innerLayoutContext = new QGridLayout();
    QGroupBox *context = new QGroupBox();
    context->setTitle("Spracherkennungs Wörter");
    contextTable = new QTableWidget();

    /* table ui */
    contextTable->verticalHeader()->setVisible(false);
    contextTable->horizontalHeader()->setVisible(false);
    contextTable->setRowCount(0);

    QStringList *headerListContext = new QStringList();
    headerListContext->append("Ausdruck/Wort");
    headerListContext->append("Entfernen");

    contextTable->setColumnCount(headerListContext->length());
    contextTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QPushButton *addNewContext = new QPushButton("Neuer Ausdruck");
    innerLayoutContext->addWidget(contextTable);
    innerLayoutContext->addWidget(addNewContext);
    context->setLayout(innerLayoutContext);

    /* connect buttons */
    connect(addNew, SIGNAL (released()), this, SLOT (addNewPP()));
    connect(addNewContext, SIGNAL (released()), this, SLOT (addNewContext()));

    /* setup main window */
    mainLayout = new QGridLayout();
    mainLayout->addWidget(pp);
    mainLayout->addWidget(context);
    QWidget *mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);
}

void ServerConfig::addNewPP(){

    QStringList *sl = new QStringList();
    sl->append("Schlüsselword");
    sl->append("Ersetzung");

    QWidgetList *wl = new QWidgetList();
    wl->append(new QLineEdit());
    wl->append(new QLineEdit());

    MultiValueInputDialog *dialog = new MultiValueInputDialog(sl, wl);
    if (dialog->exec() == QDialog::Accepted) {
        auto keyword = static_cast<QLineEdit*>(wl->at(0));
        auto replace = static_cast<QLineEdit*>(wl->at(0));

        if(!keyword->text().isEmpty() && !replace->text().isEmpty()){
            sc->submitPostProcessorChange(keyword->text(), replace->text());
        }
    }
}

void ServerConfig::askFlushServerCache(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Server Cache",
               "Sollen alte Transcripte auf dem Server, die ohne diese Konfiguration erstellt wurden gelöscht werden?",
               QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        sc->flushCache();
    }
}

void ServerConfig::addNewContext()
{
    QStringList *sl = new QStringList();
    sl->append("Ausdruck");

    QWidgetList *wl = new QWidgetList();
    wl->append(new QLineEdit());

    MultiValueInputDialog *dialog = new MultiValueInputDialog(sl, wl);
    if (dialog->exec() == QDialog::Accepted) {
        auto lineEdit = static_cast<QLineEdit*>(wl->at(0));
        if(!lineEdit->text().isEmpty()){
            sc->submitSpeechContextPhraseChange(lineEdit->text());
            askFlushServerCache();
        }
    }
}

QWidget* ServerConfig::loatListItemUiForm()
{
    QUiLoader loader;

    QFile file(":/forms/serverconfigitem.ui");
    file.open(QFile::ReadOnly);

    QWidget *formWidget = loader.load(&file, this);
    file.close();

    return formWidget;
}

void ServerConfig::finishedRequest(QNetworkReply *reply){
    if(reply->error() != QNetworkReply::NoError){
        qDebug("unified sc fail..");
        return;
    }

    QString addPP = sc->buildURLFromLocation(PP_EDIT);
    QString addContext = sc->buildURLFromLocation(CONTEXT_EDIT);
    QString flushCache = sc->buildURLFromLocation(FLUSH_SERVER_CACHE);

    if(QString::compare(reply->url().toString(), addPP) == 0){
        sc->getUnifiedServerConfig();
    }else if(QString::compare(reply->url().toString(), addContext) == 0){
        sc->getUnifiedServerConfig();
    }else if(QString::compare(reply->url().toString(), flushCache) == 0){
        qDebug("WTF");
        QMessageBox msgBox;
        msgBox.setText("Server Cache Gelöscht");
        QJsonObject jsonFlushCache = QJsonDocument::fromJson(reply->readAll()).object();
        QJsonArray removals = jsonFlushCache["removals"].toArray();
        QString display = "";
        for(int i = 0; i < removals.size(); i++){
            display += removals[i].toString();
            display += "<br>";
        }
        msgBox.setInformativeText(display);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }else{
        /* this is the unified server config query */
        /* get filename and tracking id from replay */
        QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();

        auto keywordMap = json["keyword-map"].toObject();
        auto phrases = json["phrases"].toArray();

        contextTable->clear();
        contextTable->setRowCount(0);
        ppTable->clear();
        ppTable->setRowCount(0);

        for(int i = 0; i < phrases.size(); i++){
            contextTable->insertRow(i);
            contextTable->setItem(i, 0, new QTableWidgetItem(phrases.at(i).toString()));
            auto *deleteButtonLayout = new QGridLayout();
            auto *deleteCell = new QWidget();
            auto *deleteButton = new QPushButton("Entfernen");
            deleteButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            deleteButtonLayout->addWidget(deleteButton);
            deleteButtonLayout->setContentsMargins(0,0,0,0);
            deleteCell->setLayout(deleteButtonLayout);

            contextTable->setCellWidget(i, 1, deleteCell);
        }

        for(int i = 0; i < keywordMap.keys().size(); i++){
            auto key = keywordMap.keys().at(i);
            ppTable->insertRow(i);
            ppTable->setItem(i, 0, new QTableWidgetItem(key));
            ppTable->setItem(i, 1, new QTableWidgetItem(keywordMap[key].toString()));
            auto *deleteButtonLayout = new QGridLayout();
            auto *deleteCell = new QWidget();
            auto *deleteButton = new QPushButton("Entfernen");
            deleteButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            deleteButtonLayout->addWidget(deleteButton);
            deleteButtonLayout->setContentsMargins(0,0,0,0);
            deleteCell->setLayout(deleteButtonLayout);

            ppTable->setCellWidget(i, 2, deleteCell);
        }

        this->repaint();
    }
}

ServerConfig::~ServerConfig()
{
}

