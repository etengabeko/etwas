TEMPLATE = app
PROJECT = retranslate-srv
TARGET = $$PROJECT

QT = \
    core \
    network

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

HEADERS = \
    $$PWD/src/retranslateserver.h

SOURCES = \
    $$PWD/src/retranslateserver.cpp \
    $$PWD/src/main.cpp

INSTALLS += \
    target
