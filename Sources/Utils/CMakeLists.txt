cmake_minimum_required(VERSION 3.10)
project(Utils)

set(HEADERS 
    Logger.hpp 
    Defaults.hpp
    Interfaces.hpp
    Files.h)

set(SOURCES 
    Files.cpp)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
add_library(lucid::Utils ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)

find_package(fmt REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE fmt::fmt)

find_package(rang REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE rang::rang)

find_package(glm REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE glm::glm)
