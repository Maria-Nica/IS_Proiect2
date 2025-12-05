cmake_minimum_required(VERSION 3.16)
project(UnitTests LANGUAGES CXX)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

file(GLOB_RECURSE TEST_SOURCES "*.cpp" "*.h")

add_executable(UnitTests ${TEST_SOURCES})

add_dependencies(UnitTests LogicLib)

target_include_directories(UnitTests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Logic
)

add_subdirectory(${CMAKE_SOURCE_DIR}/extern/googletest ${CMAKE_BINARY_DIR}/googletest_build)

target_link_libraries(UnitTests PRIVATE LogicLib gtest_main gmock_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(UnitTests)
add_test(NAME UnitTests COMMAND $<TARGET_FILE:UnitTests>)

# Convenience target so IDEs can run all tests via a build/run target
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Run all CTest tests"
)
