#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>
#include <QPushButton>
#include <QSettings>
#include <QTableWidget>
#include <serverconnection.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void importFile();
    void requestFinished(QNetworkReply*);
    void queryStatusAllWrapper();
    void openContainingDir();
    void openConfigurationWindow();
    void appyConfigChanges();
    void openAboutWindow();
    void openSpeechConfigWindow();
private:
    Ui::MainWindow *ui;
    QTableWidget *tw;
    QPushButton *button;
    QAction *standardsMenu;
    QSettings *mySettings;
    ServerConnection *serverConnection;
    void updateList(QNetworkReply *reply);
    void addTrackingToList(QNetworkReply *reply);
    void showNotification(QString str);
    void saveTranscript(QNetworkReply *reply);
    void handleInitialSettings();
    void submitFileSlot(QString filename);
};

#endif // MAINWINDOW_H
