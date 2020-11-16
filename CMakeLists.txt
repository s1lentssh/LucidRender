cmake_minimum_required(VERSION 3.10)

project(LucidRender 
    VERSION 0.1
    DESCRIPTION "Minimalistic render application on Vulkan and D3D12")

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan.cmake) # latest from github
conan_cmake_run(
    REQUIRES
        fmt/7.0.3
        rang/3.1.0
        glfw/3.3.2
        stb/20200203
        imgui/1.79
        shaderc/2019.0

    BASIC_SETUP
    CMAKE_TARGETS

    OPTIONS
        shaderc:shared=True

    BUILD missing
    GENERATORS cmake_find_package)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})
set(RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resources)

add_subdirectory(Sources)
