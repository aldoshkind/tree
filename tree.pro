#-------------------------------------------------
#
# Project created by QtCreator 2016-06-30T18:12:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tree
TEMPLATE = app

CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
    filepath_utils.cpp \
    resource.cpp \
    tree_node.cpp \
    property_listener.cpp

HEADERS  += \
    filepath_utils.h \
    tree_node.h \
    resource.h \
    property_listener.h \
    tree_node_inherited.h
