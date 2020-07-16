#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>
#include <QPushButton>

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
    void fileSubmissionFinished(QNetworkReply*);

private:
    Ui::MainWindow *ui;
    QPushButton *button;
    void submitFile(QString filename);
};

#endif // MAINWINDOW_H
