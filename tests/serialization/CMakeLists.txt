cmake_minimum_required(VERSION 2.8.12)
project(serialization-test)

set(ROOT_PROJECT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories(${ROOT_PROJECT_SRC_DIR})

set(ROOT_PROJECT_HEADERS
    ${ROOT_PROJECT_SRC_DIR}/protocol/protocol.h
    ${ROOT_PROJECT_SRC_DIR}/protocol/protocol_private.h
    ${ROOT_PROJECT_SRC_DIR}/protocol/types.h
)

set(HEADERS
    ${ROOT_PROJECT_HEADERS}
    src/basictest.h
    src/beginlog.h
    src/blinkoptions.h
    src/brightoptions.h
    src/buttonsstate.h
    src/currenttime.h
    src/deviceaddress.h
    src/deviceidentity.h
    src/displayimages.h
    src/displayoptions.h
    src/imagedata.h
    src/nextlog.h
)

set(SOURCES
    ${ROOT_PROJECT_SRC_DIR}/protocol/protocol.cpp
    ${ROOT_PROJECT_SRC_DIR}/protocol/protocol_private.cpp
    src/basictest.cpp
    src/beginlog.cpp
    src/blinkoptions.cpp
    src/brightoptions.cpp
    src/buttonsstate.cpp
    src/currenttime.cpp
    src/deviceaddress.cpp
    src/deviceidentity.cpp
    src/displayimages.cpp
    src/displayoptions.cpp
    src/imagedata.cpp
    src/nextlog.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
add_test(${PROJECT_NAME} ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Gui Qt5::Test)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
        ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/sbin"
)
