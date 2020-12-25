cmake_minimum_required(VERSION 3.10)
project(Vulkan)

set(HEADERS
    VulkanDevice.cpp
    VulkanRender.cpp 
    VulkanInstance.cpp
    VulkanSurface.cpp
    VulkanSwapchain.cpp
    VulkanPipeline.cpp
    VulkanShader.cpp
    VulkanRenderPass.cpp
    VulkanCommandPool.cpp
    VulkanBuffer.cpp
    VulkanVertex.cpp
    VulkanEntity.cpp
    VulkanDescriptorPool.cpp)

set(SOURCES
    VulkanDevice.h
    VulkanRender.h 
    VulkanInstance.h
    VulkanSurface.h
    VulkanSwapchain.h
    VulkanPipeline.h
    VulkanShader.h
    VulkanRenderPass.h
    VulkanCommandPool.h
    VulkanBuffer.h
    VulkanVertex.h
    VulkanEntity.h
    VulkanDescriptorPool.h)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
add_library(lucid::Vulkan ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_WIN32_KHR NOMINMAX)
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_XLIB_KHR)
endif()

find_package(Vulkan REQUIRED)
find_package(shaderc REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    Vulkan::Vulkan 
    Utils
    shaderc::shaderc)