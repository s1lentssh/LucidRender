cmake_minimum_required(VERSION 3.10)

project(LucidRender 
    VERSION 0.1
    DESCRIPTION "Minimalistic render application on Vulkan and D3D12")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/Install CACHE PATH "Set Install prefix to default" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/conan.cmake) # latest from github
conan_cmake_run(
    REQUIRES
        fmt/7.0.3
        rang/3.1.0
        glfw/3.3.2
        stb/20200203
        imgui/1.88
        shaderc/2019.0
        glm/0.9.9.8
        tinyobjloader/1.0.6
        boost/1.79.0

    BASIC_SETUP
    CMAKE_TARGETS

    OPTIONS
        shaderc:shared=False

    IMPORTS "./res/bindings, *.cpp -> generated/imgui"
    IMPORTS "./res/bindings, *.h -> generated/imgui"

    BUILD missing
    GENERATORS cmake_find_package)

# Includes
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/macros.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resources)

add_subdirectory(Lucid)
add_subdirectory(Standalone)
