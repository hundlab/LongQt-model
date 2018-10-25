#-------------------------------------------------
#
# Project created by QtCreator 2015-10-19T09:57:24
#
#-------------------------------------------------

QT       = core concurrent

TARGET = LongQt-model
TEMPLATE = lib

QMAKE_MAC_SDK = macosx10.12

CONFIG += c++11 staticlib

include(./src/model.pri)

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

# Install files
## header files
INSTALL_DIR = "C:/Progra~2/LongQt-model/"
i =0
for(header, HEADERS) {
    tempName = $$dirname(header)
    endDir = $$INSTALL_DIR"include"$$replace(tempName,$$PWD,"")"/"
    eval(HEADER_INSTALLS_$${i}.path = $$endDir)
#message($$eval(HEADER_INSTALLS_$${i}.path))
    eval(HEADER_INSTALLS_$${i}.files = $$header)
    eval(INSTALLS += HEADER_INSTALLS_$$i)
    i = $$str_size($$INSTALLS)
}

## config files (cmake and qmake)
CMAKE_INSTALLS.path = $$INSTALL_DIR"/share/cmake/LongQt-model/"
CMAKE_INSTALLS.files = $$PWD"/LongQt-modelConfig.cmake"
QMAKE_INSTALLS.path = $$INSTALL_DIR"/share/qmake/LongQt-model/"
QMAKE_INSTALLS.files = ""
INSTALLS += CMAKE_INSTALLS

## shared and static lib
target.path = $$INSTALL_DIR
dlltarget.path = $$INSTALL_DIR
#message($$HEADER_INSTALLS.files)
INSTALLS += target
