cmake_minimum_required(VERSION 3.21)
project(Utils)

# Create target
file(GLOB_RECURSE SOURCES *.cpp *.h) 
add_library(${PROJECT_NAME} ${SOURCES})
add_library(Lucid::Utils ALIAS ${PROJECT_NAME})

# Find packages
find_package(fmt CONFIG REQUIRED)
find_path(RANG_INCLUDE_DIRS "rang.hpp")
find_package(Stb REQUIRED)
find_package(tinyobjloader REQUIRED)
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")

# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    fmt::fmt
    tinyobjloader::tinyobjloader
    Lucid::Core
)

target_include_directories(${PROJECT_NAME} PRIVATE ${RANG_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${TINYGLTF_INCLUDE_DIRS})

IF(APPLE)
    add_subdirectory(MacOS)
    target_link_libraries(${PROJECT_NAME} PUBLIC Lucid::MacOS)
endif()

# Set compile options
SetMaxWarningLevel(${PROJECT_NAME})
SetWindowsVersion(${PROJECT_NAME})
SetLucidVersion(${PROJECT_NAME})
