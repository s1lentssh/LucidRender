cmake_minimum_required(VERSION 3.10)
project(Utils)

set(HEADERS 
    Logger.hpp
    Defaults.hpp
    Files.h)

set(SOURCES 
    Logger.cpp
    Files.cpp)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
add_library(Lucid::Utils ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)

find_package(fmt REQUIRED)
find_package(rang REQUIRED)
find_package(stb REQUIRED)
find_package(tinyobjloader REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    fmt::fmt
    rang::rang
    stb::stb
    tinyobjloader::tinyobjloader
    Lucid::Core)

IF(APPLE)
    add_subdirectory(MacOS)
    target_link_libraries(${PROJECT_NAME} PUBLIC Lucid::MacOS)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC NOMINMAX)
