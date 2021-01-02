cmake_minimum_required(VERSION 3.10)
project(Core)

set(SOURCES 
    InputController.cpp
    Camera.cpp
    Entity.cpp
    Scene.cpp
    Engine.cpp)

set(HEADERS 
    Mesh.h 
    Vertex.h 
    UniformBufferObject.h 
    Interfaces.h 
    Texture.h 
    Entity.h 
    Camera.h
    Scene.h
    InputController.h
    Engine.h)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
add_library(lucid::Core ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)

find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    glm::glm
    Vulkan)