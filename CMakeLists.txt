cmake_minimum_required(VERSION 3.10)
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

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/Install 
        CACHE PATH "Set Install prefix to default" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# Includes
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/macros.cmake)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
        "${CMAKE_BINARY_DIR}/conan.cmake"
        TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

# Conan
conan_cmake_run(
    REQUIRES
        fmt/9.1.0
        rang/3.2
        glfw/3.3.8
        stb/20200203
        imgui/1.88
        shaderc/2019.0
        glm/0.9.9.8
        tinyobjloader/1.0.6
        boost/1.79.0

    BASIC_SETUP
    CMAKE_TARGETS

    OPTIONS
        shaderc*:shared=False

    IMPORTS "./res/bindings, *.cpp -> generated/imgui"
    IMPORTS "./res/bindings, *.h -> generated/imgui"

    BUILD missing
    GENERATORS cmake_find_package)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

# Subdirectories
add_subdirectory(Sources)
add_subdirectory(Standalone)
