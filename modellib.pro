#-------------------------------------------------
#
# Project created by QtCreator 2015-10-19T09:57:24
#
#-------------------------------------------------

QT       += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = model
TEMPLATE = lib

QMAKE_MAC_SDK = macosx10.12

CONFIG += c++11 debug
CONFIG -= release

include(modellib/model.pri)

linux {
    debug {
        QMAKE_CXXFLAGS += -g -O0
        QMAKE_CXXFLAGS_RELEASE -= -O2
        TARGET =$$TARGET"-debug"
    }
    release {
        DEFINES += QT_NO_DEBUG_OUTPUT
        TARGET =$$TARGET"-release"
    }
    profile {
        TARGET =$$TARGET"-profile"
    }

    DESTDIR = ./build
    OBJECTS_DIR = ./build/obj
    MOC_DIR = ./build/obj
    RCC_DIR = ./build/obj
    UI_DIR = ./build/obj
}
