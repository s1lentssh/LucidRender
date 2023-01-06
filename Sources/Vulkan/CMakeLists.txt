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
    VulkanEntity.cpp
    VulkanDescriptorPool.cpp
    VulkanImage.cpp
    VulkanSampler.cpp
    VulkanMesh.cpp
    VulkanDescriptorSet.cpp)


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
    VulkanEntity.h
    VulkanDescriptorPool.h
    VulkanImage.h
    VulkanSampler.h
    VulkanMesh.h
    VulkanDescriptorSet.h)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
add_library(Lucid::Vulkan ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/../)

find_package(imgui REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_library(ImguiGenerated
    ${CMAKE_BINARY_DIR}/generated/imgui/imgui_impl_glfw.cpp
    ${CMAKE_BINARY_DIR}/generated/imgui/imgui_impl_vulkan.cpp
)
target_include_directories(ImguiGenerated SYSTEM INTERFACE
    ${CMAKE_BINARY_DIR}/generated
)
target_link_libraries(ImguiGenerated PRIVATE 
    imgui::imgui
    Vulkan::Vulkan
    glfw::glfw
)
target_link_libraries(${PROJECT_NAME} PRIVATE ImguiGenerated)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_WIN32_KHR NOMINMAX)
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_XCB_KHR)
endif()

IF(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_METAL_EXT)
endif()

SetCompileOptions(${PROJECT_NAME})

find_package(shaderc REQUIRED)
find_package(Boost REQUIRED COMPONENTS log)

target_link_libraries(${PROJECT_NAME} PUBLIC 
    Vulkan::Vulkan 
    Utils
    shaderc::shaderc
    Lucid::Core
    glfw::glfw
    Boost::log
)
