TEMPLATE = app
PROJECT = msg-debugger
TARGET = $$PROJECT

QT = \
    core \
    gui \
    widgets \
    network

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
UI_DIR = $$BUILD_DIR/ui
DESTDIR = $$BUILD_DIR/sbin

isEmpty(PREFIX) {
    PREFIX = $$PWD
}

# installs
target.path = $$PREFIX/sbin

ROOT_PROJECT_SRC_DIR = $$PWD/../../src
INCLUDEPATH += $$ROOT_PROJECT_SRC_DIR

ROOT_PROJECT_HEADERS = \
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol.h \
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol_private.h \
    $$ROOT_PROJECT_SRC_DIR/protocol/types.h \
    $$ROOT_PROJECT_SRC_DIR/ioservice/inputcontroller.h \
    $$ROOT_PROJECT_SRC_DIR/ioservice/inputcontroller_private.h \
    $$ROOT_PROJECT_SRC_DIR/ioservice/transport.h \
    $$ROOT_PROJECT_SRC_DIR/ioservice/transport_private.h \
    $$ROOT_PROJECT_SRC_DIR/storage/imagestorage.h

HEADERS = \
    $$ROOT_PROJECT_HEADERS \
    $$PWD/src/mainwindow.h

SOURCES = \
    # root project
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol.cpp \
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol_private.cpp \
    $$ROOT_PROJECT_SRC_DIR/ioservice/inputcontroller.cpp \
    $$ROOT_PROJECT_SRC_DIR/ioservice/inputcontroller_private.cpp \
    $$ROOT_PROJECT_SRC_DIR/ioservice/transport.cpp \
    $$ROOT_PROJECT_SRC_DIR/ioservice/transport_private.cpp \
    $$ROOT_PROJECT_SRC_DIR/storage/imagestorage.cpp \
    # main
    $$PWD/src/mainwindow.cpp \
    $$PWD/src/main.cpp

FORMS = \
    $$PWD/src/mainwindow.ui

INSTALLS += \
    target
