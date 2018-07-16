TEMPLATE = app
PROJECT = rw-settings
TARGET = $$PROJECT

QT = \
    core \
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
    # settings
    $$ROOT_PROJECT_SRC_DIR/settings/displaysettings.h \
    $$ROOT_PROJECT_SRC_DIR/settings/settings.h \
    $$ROOT_PROJECT_SRC_DIR/settings/settings_private.h

HEADERS = \
    $$ROOT_PROJECT_HEADERS

SOURCES = \
    # root project
    $$ROOT_PROJECT_SRC_DIR/settings/displaysettings.cpp \
    $$ROOT_PROJECT_SRC_DIR/settings/settings.cpp \
    $$ROOT_PROJECT_SRC_DIR/settings/settings_private.cpp \
    # main
    $$PWD/src/main.cpp

INSTALLS += \
    target
