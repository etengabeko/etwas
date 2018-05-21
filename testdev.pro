TEMPLATE = app
PROJECT = testdev
TARGET = $$PROJECT

QT = \
    core \
    gui \
    widgets

CONFIG += warn_on

BUILD_DIR = $$PWD/build
DESTDIR = $$BUILD_DIR
OBJECTS_DIR = $$BUILD_DIR/obj
MOC_DIR = $$BUILD_DIR/moc
UI_DIR = $$BUILD_DIR/ui

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS *= -Wall -Wextra -Werror -pedantic-errors

HEADERS = \
    # logger
    src/logger/logger.h \
    # protocol
    src/protocol/protocol.h

SOURCES = \
    # logger
    src/logger/logger.cpp \
    # protocol
    src/protocol/protocol.cpp \
    # main
    src/main.cpp

FORMS =
