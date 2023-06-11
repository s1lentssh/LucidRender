cmake_minimum_required(VERSION 3.21)
project(Utils)

# Create target
file(GLOB_RECURSE SOURCES *.cpp *.h) 
add_library(${PROJECT_NAME} ${SOURCES})
add_library(Lucid::Utils ALIAS ${PROJECT_NAME})

# Find packages
find_package(fmt REQUIRED)
find_package(rang REQUIRED)
find_package(stb REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(TinyGLTF)

# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    fmt::fmt
    rang::rang
    stb::stb
    tinyobjloader::tinyobjloader
    Lucid::Core
    TinyGLTF::TinyGLTF
)

IF(APPLE)
    add_subdirectory(MacOS)
    target_link_libraries(${PROJECT_NAME} PUBLIC Lucid::MacOS)
endif()

# Set compile options
SetMaxWarningLevel(${PROJECT_NAME})
SetWindowsVersion(${PROJECT_NAME})
SetLucidVersion(${PROJECT_NAME})
