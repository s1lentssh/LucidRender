cmake_minimum_required(VERSION 2.8)
project(MacOS)

# Create target
file(GLOB SOURCES *.h *.m)
add_library(${PROJECT_NAME} ${SOURCES})
add_library(Lucid::MacOS ALIAS ${PROJECT_NAME})

# Set compile options
SetMaxWarningLevel(${PROJECT_NAME})
SetWindowsVersion(${PROJECT_NAME})
SetLucidVersion(${PROJECT_NAME})
