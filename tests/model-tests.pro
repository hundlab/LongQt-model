include(gtest_dependency.pri)
include(../model.pri)

QT       = core

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
TARGET = model-tester

HEADERS +=     \
    celltest.h \
    protocoltest.h

SOURCES +=     main.cpp
