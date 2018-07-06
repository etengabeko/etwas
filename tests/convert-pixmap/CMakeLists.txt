cmake_minimum_required(VERSION 2.8.12)
project(convert-pixmap)

set(ROOT_PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories(${ROOT_PROJECT_SRC_DIR})

set(ROOT_PROJECT_HEADERS
    ${ROOT_PROJECT_SRC_DIR}/storage/imagestorage.h
)

set(HEADERS
    ${ROOT_PROJECT_HEADERS}
)

set(SOURCES
    ${ROOT_PROJECT_SRC_DIR}/storage/imagestorage.cpp
    src/main.cpp
)

qt5_add_resources(RESOURCES
    src/resources.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})
add_test(${PROJECT_NAME} ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Gui Qt5::Test)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
        ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/sbin"
)

