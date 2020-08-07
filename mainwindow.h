#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>
#include <QPushButton>
#include <QTableWidget>

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
    void queryStatusAll();
    void openContainingDir();
private:
    Ui::MainWindow *ui;
    QTableWidget *tw;
    QPushButton *button;
    QByteArray authHeaderData;
    QNetworkAccessManager *networkManager;
    void submitFile(QString filename);
    void setAuthHeader(const QString username, const QString password);
    void updateList(QNetworkReply *reply);
    void addTrackingToList(QNetworkReply *reply);
    void showNotification(QString str);
    void queryTransscript(QString trackingId);
    void saveTranscript(QNetworkReply *reply);
};

#endif // MAINWINDOW_H
