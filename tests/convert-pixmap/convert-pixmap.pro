TEMPLATE = app
PROJECT = convert-pixmap
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
RCC_DIR = $$BUILD_DIR/rcc
DESTDIR = $$BUILD_DIR/sbin

isEmpty(PREFIX) {
    PREFIX = $$PWD
}

# installs
target.path = $$PREFIX/sbin

ROOT_PROJECT_SRC_DIR = $$PWD/../../src
INCLUDEPATH += $$ROOT_PROJECT_SRC_DIR

ROOT_PROJECT_HEADERS = \
    # storage
    $$ROOT_PROJECT_SRC_DIR/storage/imagestorage.h

HEADERS = \
    $$ROOT_PROJECT_HEADERS

SOURCES = \
    # root project
    $$ROOT_PROJECT_SRC_DIR/storage/imagestorage.cpp \
    # main
    $$PWD/src/main.cpp

RESOURCES = \
    $$PWD/src/resources.qrc

INSTALLS += \
    target
