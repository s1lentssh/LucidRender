cmake_minimum_required(VERSION 3.10)
project(Application)

set(SOURCES Source.cpp Window.cpp)
set(HEADERS Window.h)

find_package(glfw REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} 
    lucid::Utils
    lucid::Vulkan
    glfw::glfw
    Vulkan::Vulkan)

target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_EXPOSE_NATIVE_WIN32)