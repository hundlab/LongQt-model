
include(gtest_dependency.pri)
include(../src/model.pri)

QT       = core concurrent

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
TARGET = model-tester

HEADERS +=     \
    celltest.h \
    protocoltest.h

SOURCES +=     main.cpp
