#-------------------------------------------------
#
# Project created by QtCreator 2020-07-16T16:18:39
#
#-------------------------------------------------

GIT_STR_VERSION = $$system(git describe)
DATE_STR = $$system(date)
HOSTNAME_STR = $$system(hostname)

QT       += core gui network uitools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = speech-server-client-qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += "GIT_VERSION=\"\\\"$$GIT_STR_VERSION\\\"\""
DEFINES += "BUILD_DATE=\"\\\"$$DATE_STR\\\"\""
DEFINES += "BUILD_HOST=\"\\\"$$HOSTNAME_STR\\\"\""

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    notificationwidget.cpp \
    settings.cpp \
    about.cpp \
    serverconnection.cpp \
    serverconfig.cpp \
    multivalueinputdialog.cpp \
    pushbuttonwithposition.cpp

HEADERS += \
        mainwindow.h \
    notificationwidget.h \
    settings.h \
    settingkeys.h \
    about.h \
    serverconnection.h \
    serverconfig.h \
    urls.h \
    multivalueinputdialog.h \
    pushbuttonwithposition.h

FORMS += \
        mainwindow.ui \
    listItemServeConfig.ui \
    serverconfigitem.ui \
    settings.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    defaultres.qrc
