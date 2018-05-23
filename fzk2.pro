TEMPLATE = app
PROJECT = fzk2
TARGET = $$PROJECT

QT = \
    core \
    gui \
    widgets

CONFIG += warn_on
QMAKE_CXXFLAGS *= -Wall -Wextra -Werror -pedantic-errors
QMAKE_CXXFLAGS += -std=c++11

BUILD_DIR = $$PWD/build
OBJECTS_DIR = $$BUILD_DIR/obj
MOC_DIR = $$BUILD_DIR/moc
UI_DIR = $$BUILD_DIR/ui
DESTDIR = $$BUILD_DIR/bin

isEmpty(PREFIX) {
    PREFIX = $$PWD
    CONFIG_PATH = $$PREFIX/config
}
else {
    CONFIG_PATH = $$PREFIX/etc/$$PROJECT
    DEFINES += CONFIG_PATH=\\\"$$CONFIG_PATH\\\"
}

# installs
target.path = $$PREFIX/bin

# config
OTHER_FILES += $$PWD/config/settings.xml
config.path = $$PREFIX/etc/$$PROJECT
config.files += $$OTHER_FILES

HEADERS = \
    # logger
    $$PWD/src/logger/logger.h \
    # protocol
    $$PWD/src/protocol/protocol.h

SOURCES = \
    # logger
    $$PWD/src/logger/logger.cpp \
    # protocol
    $$PWD/src/protocol/protocol.cpp \
    $$PWD/src/main.cpp

FORMS =

INSTALLS += \
    target \
    config
