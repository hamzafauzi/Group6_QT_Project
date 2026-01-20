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
)

# Remove mainwindow.ui since we're building UI programmatically

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ArduinoSimulator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources/images/button_down.png resources/images/button_up.png resources/images/led_off.png resources/images/led_on.png resources/images/resistor.png
        resources.qrc
        buttoncomponent.cpp
        component.cpp
        ledcomponent.cpp
        buttoncomponent.h
        component.h
        ledcomponent.h
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
