cmake_minimum_required(VERSION 2.8.12)
project(display-control)

set(ROOT_PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories(${ROOT_PROJECT_SRC_DIR})

set(ROOT_PROJECT_HEADERS
    ${ROOT_PROJECT_SRC_DIR}/storage/imagestorage.h
    ${ROOT_PROJECT_SRC_DIR}/ui/displaycontrolwidget.h
    ${ROOT_PROJECT_SRC_DIR}/ui/displayoptionswidget.h
    ${ROOT_PROJECT_SRC_DIR}/ui/subwindow.h
)

set(HEADERS
    ${ROOT_PROJECT_HEADERS}
)

set(SOURCES
    ${ROOT_PROJECT_SRC_DIR}/storage/imagestorage.cpp
    ${ROOT_PROJECT_SRC_DIR}/ui/displaycontrolwidget.cpp
    ${ROOT_PROJECT_SRC_DIR}/ui/displayoptionswidget.cpp
    ${ROOT_PROJECT_SRC_DIR}/ui/subwindow.cpp
    src/main.cpp
)

qt5_wrap_ui(FORMS
    ${ROOT_PROJECT_SRC_DIR}/ui/displaycontrol.ui
    ${ROOT_PROJECT_SRC_DIR}/ui/displayoptions.ui
)

qt5_add_resources(RESOURCES
    src/resources.qrc
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCES})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
        ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/sbin"
)


