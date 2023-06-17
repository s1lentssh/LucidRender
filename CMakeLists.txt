cmake_minimum_required(VERSION 3.21)
project(LucidRender 
    VERSION 0.2.0
    DESCRIPTION "Pre-Alpha")

# Setup
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(LUCID_RENDER_VERSION "${CMAKE_PROJECT_VERSION} ${CMAKE_PROJECT_DESCRIPTION}")

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resources)

# Defaults
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/Install 
        CACHE PATH "Set Install prefix to default" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# Includes
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/macros.cmake)

# Subdirectories
add_subdirectory(Sources)
add_subdirectory(Standalone)
