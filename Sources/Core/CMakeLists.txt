cmake_minimum_required(VERSION 3.10)
project(Core)

set(SOURCES 
    InputController.cpp
    Camera.cpp
    Entity.cpp
    Scene.cpp
    Engine.cpp)

set(HEADERS 
    Vertex.h 
    UniformBufferObject.h 
    Interfaces.h 
    Entity.h 
    Camera.h
    Scene.h
    Types.h
    InputController.h
    Engine.h)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
add_library(Lucid::Core ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall 
        -Wextra 
        -Wshadow 
        -Wnon-virtual-dtor 
        -Wpedantic 
        -Wconversion 
        -Wpedantic 
        -Werror
    )
endif()

find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    glm::glm
    Vulkan)