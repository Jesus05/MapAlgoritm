#-------------------------------------------------
#
# Project created by QtCreator 2014-10-14T11:30:58
#
#-------------------------------------------------

QT       += core gui

CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MapAlgorithm
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    DebugAddons.cpp

HEADERS  += mainwindow.h \
    DebugAddons.h \
    Types.h

FORMS    += mainwindow.ui
