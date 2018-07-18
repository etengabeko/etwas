cmake_minimum_required(VERSION 2.8.12)
project(msg-debugger)

set(ROOT_PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories(${ROOT_PROJECT_SRC_DIR})

set(ROOT_PROJECT_HEADERS
    ${ROOT_PROJECT_SRC_DIR}/protocol/protocol.h
    ${ROOT_PROJECT_SRC_DIR}/protocol/protocol_private.h
    ${ROOT_PROJECT_SRC_DIR}/protocol/types.h
    ${ROOT_PROJECT_SRC_DIR}/ioservice/inputcontroller.h
    ${ROOT_PROJECT_SRC_DIR}/ioservice/inputcontroller_private.h
    ${ROOT_PROJECT_SRC_DIR}/ioservice/transport.h
    ${ROOT_PROJECT_SRC_DIR}/ioservice/transport_private.h
)

set(HEADERS
    ${ROOT_PROJECT_HEADERS}
)

set(SOURCES
    ${ROOT_PROJECT_SRC_DIR}/protocol/protocol.cpp
    ${ROOT_PROJECT_SRC_DIR}/protocol/protocol_private.cpp
    ${ROOT_PROJECT_SRC_DIR}/ioservice/inputcontroller.cpp
    ${ROOT_PROJECT_SRC_DIR}/ioservice/inputcontroller_private.cpp
    ${ROOT_PROJECT_SRC_DIR}/ioservice/transport.cpp
    ${ROOT_PROJECT_SRC_DIR}/ioservice/transport_private.cpp
    src/main.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} Qt5::Network Qt5::Gui Qt5::Core)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
        ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/sbin"
)
