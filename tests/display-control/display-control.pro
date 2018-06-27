TEMPLATE = app
PROJECT = display-control
TARGET = $$PROJECT

QT = \
    core \
    gui \
    widgets

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
    $$ROOT_PROJECT_SRC_DIR/logger/logger.h \
    $$ROOT_PROJECT_SRC_DIR/ui/displaycontrolwidget.h \
    $$ROOT_PROJECT_SRC_DIR/ui/displayoptionswidget.h \
    $$ROOT_PROJECT_SRC_DIR/ui/subwindow.h

HEADERS = \
    $$ROOT_PROJECT_HEADERS

SOURCES = \
    # root project
    $$ROOT_PROJECT_SRC_DIR/logger/logger.cpp \
    $$ROOT_PROJECT_SRC_DIR/ui/displaycontrolwidget.cpp \
    $$ROOT_PROJECT_SRC_DIR/ui/displayoptionswidget.cpp \
    $$ROOT_PROJECT_SRC_DIR/ui/subwindow.cpp \
    # main
    $$PWD/src/main.cpp

FORMS = \
    $$ROOT_PROJECT_SRC_DIR/ui/displaycontrol.ui \
    $$ROOT_PROJECT_SRC_DIR/ui/displayoptions.ui

RESOURCES = \
    $$PWD/src/resources.qrc

INSTALLS += \
    target
