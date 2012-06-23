#-------------------------------------------------
#
# Project created by QtCreator 2012-06-09T13:28:39
#core gui
#-------------------------------------------------

QT       += network


TARGET = downloader

CONFIG += console
TEMPLATE = app


SOURCES += \
    main.cpp \
    downloadmanager.cpp \
    widget.cpp \
    task.cpp \
    newTaskDialog.cpp \
    base64.cpp

HEADERS  += \
    downloadmanager.h \
    widget.h \
    task.h \
    newTaskDialog.h \
    base64.h \
    MyNetworkReply.h

OTHER_FILES += \
    download.png

RESOURCES += \
    resources.qrc
