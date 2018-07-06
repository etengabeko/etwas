cmake_minimum_required(VERSION 2.8.12)
project(retranslate-srv)

set(HEADERS
    src/retranslateserver.h
)

set(SOURCES
    src/retranslateserver.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} Qt5::Network)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
        ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/sbin"
)


