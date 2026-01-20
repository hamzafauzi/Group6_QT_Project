cmake_minimum_required(VERSION 3.16)

project(ArduinoSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Source files
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

# Create executable
add_executable(ArduinoSimulator
    ${PROJECT_SOURCES}
)

# Link Qt libraries
target_link_libraries(ArduinoSimulator PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# macOS specific settings
if(APPLE)
    set_target_properties(ArduinoSimulator PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()
