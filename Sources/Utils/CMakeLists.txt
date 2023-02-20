cmake_minimum_required(VERSION 3.10)
project(Utils)

# Create target
file(GLOB SOURCES *.cpp *.h) 
add_library(${PROJECT_NAME} ${SOURCES})
add_library(Lucid::Utils ALIAS ${PROJECT_NAME})

# Find packages
find_package(fmt REQUIRED)
find_package(rang REQUIRED)
find_package(stb REQUIRED)
find_package(tinyobjloader REQUIRED)

# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    fmt::fmt
    rang::rang
    stb::stb
    tinyobjloader::tinyobjloader
    Lucid::Core
)

IF(APPLE)
    add_subdirectory(MacOS)
    target_link_libraries(${PROJECT_NAME} PUBLIC Lucid::MacOS)
endif()

# Set compile options
SetMaxWarningLevel(${PROJECT_NAME})
SetLucidVersion(${PROJECT_NAME})
