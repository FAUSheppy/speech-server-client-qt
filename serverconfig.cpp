#include "serverconfig.h"
#include "serverconnection.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QListWidget>
#include <QWidget>
#include <QtUiTools>

ServerConfig::ServerConfig(QWidget *parent, QSettings *settings) : QMainWindow(parent) {

    mySettings = settings;

    ServerConnection *sc = new ServerConnection(this, mySettings);
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


    /* setup main window */
    mainLayout = new QGridLayout();
    mainLayout->addWidget(pp);
    mainLayout->addWidget(context);
    QWidget *mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);
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

    /* get filename and tracking id from replay */
    QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();

    auto keywordMap = json["keyword-map"].toObject();
    auto phrases = json["phrases"].toArray();

    contextTable->clear();

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
    this->repaint();

}

ServerConfig::~ServerConfig()
{
}

