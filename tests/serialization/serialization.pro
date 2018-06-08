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
    $$PWD/src/buttonsstate.h \
    $$PWD/src/deviceidentity.h

SOURCES = \
    # root project
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol.cpp \
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol_private.cpp \
    # main
    $$PWD/src/basictest.cpp \
    $$PWD/src/buttonsstate.cpp \
    $$PWD/src/deviceidentity.cpp \
    $$PWD/src/main.cpp

INSTALLS += \
    target
