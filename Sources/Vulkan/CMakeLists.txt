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
    VulkanCommandManager.cpp)

set(SOURCES
    VulkanDevice.h
    VulkanRender.h 
    VulkanInstance.h
    VulkanSurface.h
    VulkanSwapchain.h
    VulkanPipeline.h
    VulkanShader.h
    VulkanRenderPass.h
    VulkanCommandManager.h)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
add_library(lucid::Vulkan ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)
target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_WIN32_KHR NOMINMAX)

find_package(Vulkan REQUIRED)
find_package(shaderc REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    Vulkan::Vulkan 
    Utils
    shaderc::shaderc)