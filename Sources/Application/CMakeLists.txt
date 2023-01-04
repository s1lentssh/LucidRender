cmake_minimum_required(VERSION 3.10)
project(Application)

set(SOURCES Source.cpp Window.cpp)
set(HEADERS Window.h)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(stb REQUIRED)
find_package(glm REQUIRED)

if(APPLE)
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES_DIR}/Icons/AppResource.o)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Lucid")
install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(DIRECTORY ${RESOURCES_DIR} DESTINATION .)

target_link_libraries(${PROJECT_NAME} 
    Lucid::Utils
    Lucid::Vulkan
    glfw::glfw
    Vulkan::Vulkan
    stb::stb
    glm::glm)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} Lucid::D3D12)
    target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_EXPOSE_NATIVE_WIN32)
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_EXPOSE_NATIVE_X11)
endif()

if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC GLFW_EXPOSE_NATIVE_COCOA)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework QuartzCore")
endif()

function(register_resource resource)
    configure_file(${RESOURCES_DIR}/${resource} ${CMAKE_CURRENT_BINARY_DIR}/Resources/${resource} COPYONLY)
endfunction()

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

register_resource(Shaders/Shader.vert)
register_resource(Shaders/Shader.frag)
register_resource(Icons/AppIcon.png)
register_resource(Textures/Ancient.jpg)
register_resource(Textures/VikingRoom.png)
register_resource(Models/VikingRoom.obj)

if(NOT APPLE)
    register_resource(Icons/AppResource.o)
endif()
