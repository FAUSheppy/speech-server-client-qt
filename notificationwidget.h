#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPropertyAnimation>

class NotificationWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(float popupOpacity READ getPopupOpacity WRITE setPopupOpacity)

    void setPopupOpacity(float opacity);
    float getPopupOpacity() const;

public:
    explicit NotificationWidget(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *e);

public slots:
    void setPopupText(const QString& text);
    void show();
    void fadeOut();

private slots:
    void hideAndDestroy();

private:
    QLabel label;
    QGridLayout layout;
    QPropertyAnimation animation;
    float popupOpacity;
};

#endif // NOTIFICATIONWIDGET_H
