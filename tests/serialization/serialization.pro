TEMPLATE = app
PROJECT = serialization-test
TARGET = $$PROJECT

QT = \
    core \
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

HEADERS = \
    # protocol
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol.h

SOURCES = \
    # protocol
    $$ROOT_PROJECT_SRC_DIR/protocol/protocol.cpp \
    $$PWD/src/main.cpp

INSTALLS += \
    target
