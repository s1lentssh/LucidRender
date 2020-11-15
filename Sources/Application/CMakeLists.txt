cmake_minimum_required(VERSION 3.10)
project(Application)

set(SOURCES Source.cpp Window.cpp)
set(HEADERS Window.h)

find_package(glfw REQUIRED)
find_package(Vulkan REQUIRED)
find_package(stb REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(DIRECTORY ${RESOURCES_DIR} DESTINATION .)

target_link_libraries(${PROJECT_NAME} 
    lucid::Utils
    lucid::Vulkan
    glfw::glfw
    Vulkan::Vulkan
    stb::stb)

target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_EXPOSE_NATIVE_WIN32)

file(COPY ${RESOURCES_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})