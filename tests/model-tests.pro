
include(gtest_dependency.pri)
include(../src/model.pri)

QT       = core concurrent

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
TARGET = model-tester

HEADERS +=     \
    cellutils_test.h \
    cell_test.h \
    protocol_test.h \
    measure_test.h \
    threadpool_test.h \
    runsim_test.h \
    node_test.h \
    logger_test.h

SOURCES +=     main.cpp
