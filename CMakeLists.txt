cmake_minimum_required(VERSION 3.16)
project(ArduinoSimulator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    component.cpp
    component.h
    ledcomponent.cpp
    ledcomponent.h
    buttoncomponent.cpp
    buttoncomponent.h
    resistorcomponent.cpp
    resistorcomponent.h
    simulatorpanel.cpp
    simulatorpanel.h
    resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ArduinoSimulator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resistorcomponent.h
        resistorcomponent.cpp
        simulatorpanel.cpp
        simulatorpanel.h
    )
else()
    if(ANDROID)
        add_library(ArduinoSimulator SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(ArduinoSimulator
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ArduinoSimulator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ArduinoSimulator)
endif()

set_target_properties(ArduinoSimulator PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ArduinoSimulator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ArduinoSimulator)
endif()
