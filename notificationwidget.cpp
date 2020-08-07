#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QString>

#include "notificationwidget.h"

#include <QPainter>

NotificationWidget::NotificationWidget(QWidget *parent) :
    QWidget(parent)
{
    resize(200, 50);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    animation.setTargetObject(this);
    animation.setPropertyName("popupOpacity");
    animation.setDuration(150);

    label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    layout.addWidget(&label, 0, 0);
    setLayout(&layout);
    setAttribute(Qt::WA_X11NetWmWindowTypeSplash);
}

void NotificationWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Prepare the popup dimensions
    QRect roundedRectDimensions;
    roundedRectDimensions.setX(rect().x() + 5);
    roundedRectDimensions.setY(rect().y() + 5);
    roundedRectDimensions.setWidth(rect().width() - 10);
    roundedRectDimensions.setHeight(rect().height() - 10);

    painter.setBrush(QBrush(Qt::lightGray));

    QPen pen;
    pen.setColor(Qt::gray);
    pen.setWidth(3);
    painter.setPen(pen);

    // Draw the popup body
    painter.drawRoundedRect(roundedRectDimensions, 15, 15);
}

void NotificationWidget::setPopupText(const QString &text)
{
    label.setText(text);
}

void NotificationWidget::show()
{
    setWindowOpacity(0.0);

    animation.setStartValue(0.0);
    animation.setEndValue(1.0);

    QWidget::show();

    animation.start();
}

void NotificationWidget::setPopupOpacity(float opacity)
{
    popupOpacity = opacity;
    setWindowOpacity(static_cast<double>(opacity));
}

float NotificationWidget::getPopupOpacity() const
{
    return popupOpacity;
}

void NotificationWidget::fadeOut(){
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    setGraphicsEffect(effect);
    QPropertyAnimation *a = new QPropertyAnimation(effect,"opacity");
    a->setDuration(1000); // it will took 1000ms to face out
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::OutBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);
    connect(a, SIGNAL(finished()), this, SLOT(hideAndDestroy()));
}

void NotificationWidget::hideAndDestroy(){
    this->hide();
    delete this;
}
