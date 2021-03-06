TEMPLATE = app
PROJECT = serialization-test
TARGET = $$PROJECT

QT = \
    core \
    gui \
    testlib

CONFIG += warn_on

win32-msvc* {
    CONFIG += c++11 console
    DEFINES += "NOEXCEPT=throw()"
}
else {
    QMAKE_CXXFLAGS *= -Wall -Wextra -Werror -pedantic-errors
    QMAKE_CXXFLAGS += -std=c++11
    DEFINES += "NOEXCEPT=noexcept"
}

BUILD_DIR = $$PWD/build
OBJECTS_DIR = $$BUILD_DIR/obj
MOC_DIR = $$BUILD_DIR/moc
DESTDIR = $$BUILD_DIR/sbin

isEmpty(PREFIX) {
    PREFIX = $$PWD
}

# installs
target.path = $$PREFIX/sbin

ROOT_PROJECT_SRC_DIR = $$PWD/../../src
INCLUDEPATH += $$ROOT_PROJECT_SRC_DIR

ROOT_PROJECT_HEADERS = \
    # protocol
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol.h \
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol_private.h \
    $$ROOT_PROJECT_SRC_DIR/protocol/types.h

HEADERS = \
    $$ROOT_PROJECT_HEADERS \
    # main
    $$PWD/src/basictest.h \
    $$PWD/src/beginlog.h \
    $$PWD/src/blinkoptions.h \
    $$PWD/src/brightoptions.h \
    $$PWD/src/buttonsstate.h \
    $$PWD/src/currenttime.h \
    $$PWD/src/deviceaddress.h \
    $$PWD/src/deviceidentity.h \
    $$PWD/src/displayimages.h \
    $$PWD/src/displayoptions.h \
    $$PWD/src/imagedata.h \
    $$PWD/src/nextlog.h

SOURCES = \
    # root project
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol.cpp \
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol_private.cpp \
    # main
    $$PWD/src/basictest.cpp \
    $$PWD/src/beginlog.cpp \
    $$PWD/src/blinkoptions.cpp \
    $$PWD/src/brightoptions.cpp \
    $$PWD/src/buttonsstate.cpp \
    $$PWD/src/currenttime.cpp \
    $$PWD/src/deviceaddress.cpp \
    $$PWD/src/deviceidentity.cpp \
    $$PWD/src/displayimages.cpp \
    $$PWD/src/displayoptions.cpp \
    $$PWD/src/imagedata.cpp \
    $$PWD/src/nextlog.cpp \
    $$PWD/src/main.cpp

INSTALLS += \
    target
