#-------------------------------------------------
#
# Project created by QtCreator 2021-05-18T16:26:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DPT_Pulse
TEMPLATE = app

RC_FILE = icon.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    serial/qextserialenumerator.cpp \
    serial/qextserialport.cpp \
    serial/qextserialenumerator_win.cpp \
    serial/qextserialport_win.cpp

HEADERS  += mainwindow.h \
    serial/qextserialenumerator.h \
    serial/qextserialport.h \
    serial/qextserialport_global.h \
    serial/qextserialenumerator_p.h \
    serial/qextserialport_p.h \
    DataFormat.h

FORMS    += mainwindow.ui

RESOURCES += \
    Resource.qrc

//win32:HEADERS +=
//win32:SOURCES +=
win32:DEFINES += _TTY_WIN_
win32:LIBS += -lsetupapi

OTHER_FILES += \
    serial/qextserialport.pri

