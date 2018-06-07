TEMPLATE = app
PROJECT = sendrecv-test
TARGET = $$PROJECT

QT = \
    core \
    gui \
    network \
    testlib

CONFIG += warn_on
QMAKE_CXXFLAGS *= -Wall -Wextra -Werror -pedantic-errors
QMAKE_CXXFLAGS += -std=c++11

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
    $$ROOT_PROJECT_SRC_DIR/protocol/types.h \
    # ioservice
    $$ROOT_PROJECT_SRC_DIR/ioservice/inputcontroller.h \
    $$ROOT_PROJECT_SRC_DIR/ioservice/inputcontroller_private.h \
    $$ROOT_PROJECT_SRC_DIR/ioservice/outputcontroller.h \
    $$ROOT_PROJECT_SRC_DIR/ioservice/outputcontroller_private.h \
    $$ROOT_PROJECT_SRC_DIR/ioservice/transport.h \
    $$ROOT_PROJECT_SRC_DIR/ioservice/transport_private.h

HEADERS = \
    $$ROOT_PROJECT_HEADERS \
    $$PWD/src/retranslateserver.h

SOURCES = \
    # root project
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol.cpp \
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol_private.cpp \
    $$ROOT_PROJECT_SRC_DIR/ioservice/inputcontroller.cpp \
    $$ROOT_PROJECT_SRC_DIR/ioservice/inputcontroller_private.cpp \
    $$ROOT_PROJECT_SRC_DIR/ioservice/outputcontroller.cpp \
    $$ROOT_PROJECT_SRC_DIR/ioservice/outputcontroller_private.cpp \
    $$ROOT_PROJECT_SRC_DIR/ioservice/transport.cpp \
    $$ROOT_PROJECT_SRC_DIR/ioservice/transport_private.cpp \
    # main
    $$PWD/src/retranslateserver.cpp \
    $$PWD/src/main.cpp

INSTALLS += \
    target
