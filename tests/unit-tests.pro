TEMPLATE = app
PROJECT = unit-tests
TARGET = $$PROJECT

QT = \
    core \
    testlib

CONFIG += warn_on

BUILD_DIR = $$PWD/../build
DESTDIR = $$BUILD_DIR
OBJECTS_DIR = $$BUILD_DIR/obj
MOC_DIR = $$BUILD_DIR/moc
UI_DIR = $$BUILD_DIR/ui

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS *= -Wall -Wextra -Werror -pedantic-errors

PROJECT_SRC_DIR = $$PWD/../src
INCLUDEPATH += $$PROJECT_SRC_DIR

HEADERS = \
    # protocol
    $$PROJECT_SRC_DIR/protocol/protocol.h

SOURCES = \
    # protocol
    $$PROJECT_SRC_DIR/protocol/protocol.cpp \
    # main
    $$PWD/unit-tests.cpp
