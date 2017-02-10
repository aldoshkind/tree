#-------------------------------------------------
#
# Project created by QtCreator 2016-06-30T18:12:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tree
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    filepath_utils.cpp

HEADERS  += widget.h \
    filepath_utils.h \
    tree_node.h \
    node.h \
    resource.h \
    event_printer.h
