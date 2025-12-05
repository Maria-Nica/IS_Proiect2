cmake_minimum_required(VERSION 3.16)

project(UI LANGUAGES CXX)

# Fix MSVC __cplusplus for Qt
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# Activăm generatorii automați Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Load Qt local path
include(${CMAKE_SOURCE_DIR}/cmake/QtLocal.cmake OPTIONAL)

# Find Qt6
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# UI Sources - specificăm explicit fișierele
set(UI_SOURCES
  main.cpp
    gameui.cpp
    gameui.h
    boardwidget.cpp
    boardwidget.h
)

add_executable(UIApp
    ${UI_SOURCES}
)

target_link_libraries(UIApp PRIVATE
    Qt6::Widgets
    LogicLib
)

target_include_directories(UIApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Logic
)
