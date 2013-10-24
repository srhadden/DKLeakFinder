#-------------------------------------------------
#
# Project created by QtCreator 2012-11-30T11:51:00
#
#-------------------------------------------------

#CONFIG += qesp_linux_udev
include(qextserialport/src/qextserialport.pri)

QT       += core gui

TARGET = DKLeakFinder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dkmemmoryview.cpp

HEADERS  += mainwindow.h \
    dkmemmoryview.h

FORMS    += mainwindow.ui
