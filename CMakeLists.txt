cmake_minimum_required(VERSION 3.16)

project(Logic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix pentru MSVC (__cplusplus corect)
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

enable_testing()

include(${CMAKE_SOURCE_DIR}/cmake/QtLocal.cmake OPTIONAL)

# Logic sources
file(GLOB_RECURSE LOGIC_SOURCES
    "Logic/*.cpp"
    "Logic/*.h"
)

list(FILTER LOGIC_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")
list(FILTER LOGIC_SOURCES EXCLUDE REGEX ".*/build/.*")

add_library(LogicLib STATIC ${LOGIC_SOURCES} "Logic/Cell.h" "Logic/Position.cpp")

target_include_directories(LogicLib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/Logic
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/UI/CMakeLists.txt")
    add_subdirectory(UI)
    add_subdirectory(UnitTests)
endif()
