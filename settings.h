#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMainWindow>
#include <QLineEdit>

namespace Ui {
class settings;
}

class Settings : public QMainWindow
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private slots:
    void okClose();
    void cancleClose();
private:
    Ui::settings *ui;
    QHash<QString, QLineEdit*> *configLineEditMap;
};

#endif // SETTINGS_H
