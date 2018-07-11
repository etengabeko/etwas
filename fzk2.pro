TEMPLATE = app
PROJECT = fzk2
TARGET = $$PROJECT

QT = \
    core \
    gui \
    network \
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
DESTDIR = $$BUILD_DIR/bin

isEmpty(PREFIX) {
    PREFIX = $$PWD
}

# installs
target.path = $$PREFIX/bin

# config
OTHER_FILES += $$PWD/config/settings.ini
config.path = $$PREFIX/etc/$$PROJECT
config.files += $$OTHER_FILES

INCLUDEPATH += $$PWD/src

HEADERS = \
    # logger
    $$PWD/src/logger/logger.h \
    # protocol
    $$PWD/src/protocol/protocol.h \
    $$PWD/src/protocol/protocol_private.h \
    $$PWD/src/protocol/types.h \
    # settings
    $$PWD/src/settings/displaysettings.h \
    $$PWD/src/settings/settings.h \
    $$PWD/src/settings/settings_private.h \
    # ioservice
    $$PWD/src/ioservice/transport.h \
    $$PWD/src/ioservice/transport_private.h \
    $$PWD/src/ioservice/inputcontroller.h \
    $$PWD/src/ioservice/inputcontroller_private.h \
    $$PWD/src/ioservice/outputcontroller.h \
    $$PWD/src/ioservice/outputcontroller_private.h \
    # storage
    $$PWD/src/storage/imagestorage.h \
    # ui
    $$PWD/src/ui/connectionoptionsdialog.h \
    $$PWD/src/ui/controlpanelwidget.h \
    $$PWD/src/ui/displaycontrolwidget.h \
    $$PWD/src/ui/displayoptionswidget.h \
    $$PWD/src/ui/imagestoragewidget.h \
    $$PWD/src/ui/mainwindow.h \
    $$PWD/src/ui/subwindow.h

SOURCES = \
    # logger
    $$PWD/src/logger/logger.cpp \
    # protocol
    $$PWD/src/protocol/protocol.cpp \
    $$PWD/src/protocol/protocol_private.cpp \
    # settings
    $$PWD/src/settings/displaysettings.cpp \
    $$PWD/src/settings/settings.cpp \
    $$PWD/src/settings/settings_private.cpp \
    # ioservice
    $$PWD/src/ioservice/transport.cpp \
    $$PWD/src/ioservice/transport_private.cpp \
    $$PWD/src/ioservice/inputcontroller.cpp \
    $$PWD/src/ioservice/inputcontroller_private.cpp \
    $$PWD/src/ioservice/outputcontroller.cpp \
    $$PWD/src/ioservice/outputcontroller_private.cpp \
    # storage
    $$PWD/src/storage/imagestorage.cpp \
    # ui
    $$PWD/src/ui/connectionoptionsdialog.cpp \
    $$PWD/src/ui/controlpanelwidget.cpp \
    $$PWD/src/ui/displaycontrolwidget.cpp \
    $$PWD/src/ui/displayoptionswidget.cpp \
    $$PWD/src/ui/imagestoragewidget.cpp \
    $$PWD/src/ui/mainwindow.cpp \
    $$PWD/src/ui/subwindow.cpp \
    # main
    $$PWD/src/main.cpp

FORMS = \
    $$PWD/src/ui/connectionoptions.ui \
    $$PWD/src/ui/controlpanel.ui \
    $$PWD/src/ui/displaycontrol.ui \
    $$PWD/src/ui/displayoptions.ui \
    $$PWD/src/ui/imagestorage.ui

RESOURCES = \
    $$PWD/src/resources.qrc

INSTALLS += \
    target \
    config
