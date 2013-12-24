#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T13:36:05
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = text_server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    server.cpp

HEADERS += \
    server.h

INCLUDEPATH += /root/fangbinbin/libvirt/include

LIBS += -L/root/fangbinbin/libvirt/lib -lvirt
