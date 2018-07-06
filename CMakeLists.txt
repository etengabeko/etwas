cmake_minimum_required(VERSION 2.8.12)
project(fzk2)

if (CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-DNOEXCEPT=noexcept)
    add_compile_options(-std=c++11)
    add_compile_options(-Wall -Wextra -Werror -pedantic-errors)
endif (CMAKE_COMPILER_IS_GNUCXX)

if (MSVC)
    add_definitions("-DNOEXCEPT=throw()")
    add_compile_options(-DWIN32 -DWIN64 -DUNICODE -D_UNICODE)
endif (MSVC)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Network CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCES
    src/logger/logger.cpp
    src/protocol/protocol.cpp
    src/protocol/protocol_private.cpp
    src/settings/settings.cpp
    src/settings/settings_private.cpp
    src/ioservice/transport.cpp
    src/ioservice/transport_private.cpp
    src/ioservice/inputcontroller.cpp
    src/ioservice/inputcontroller_private.cpp
    src/ioservice/outputcontroller.cpp
    src/ioservice/outputcontroller_private.cpp
    src/storage/imagestorage.cpp
    src/ui/connectionoptionsdialog.cpp
    src/ui/controlpanelwidget.cpp
    src/ui/displaycontrolwidget.cpp
    src/ui/displayoptionswidget.cpp
    src/ui/imagestoragewidget.cpp
    src/ui/mainwindow.cpp
    src/ui/subwindow.cpp
    src/main.cpp
)

set(HEADERS
    src/logger/logger.h
    src/protocol/protocol.h
    src/protocol/protocol_private.h
    src/protocol/types.h
    src/settings/settings.h
    src/settings/settings_private.h
    src/ioservice/transport.h
    src/ioservice/transport_private.h
    src/ioservice/inputcontroller.h
    src/ioservice/inputcontroller_private.h
    src/ioservice/outputcontroller.h
    src/ioservice/outputcontroller_private.h
    src/storage/imagestorage.h
    src/ui/connectionoptionsdialog.h
    src/ui/controlpanelwidget.h
    src/ui/displaycontrolwidget.h
    src/ui/displayoptionswidget.h
    src/ui/imagestoragewidget.h
    src/ui/mainwindow.h
    src/ui/subwindow.h
)

qt5_wrap_ui(FORMS
    src/ui/connectionoptions.ui
    src/ui/controlpanel.ui
    src/ui/displaycontrol.ui
    src/ui/displayoptions.ui
    src/ui/imagestorage.ui
)

qt5_add_resources(RESOURCES
    src/resources.qrc
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Network)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
        ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
)

# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/settings.ini
# 	DESTINATION ${CMAKE_INSTALL_PREFIX}/etc
# )
