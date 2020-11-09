#pragma once

#include "VulkanInstance.h"
#include <vulkan/vulkan.hpp>


namespace lucid {

class IWindow;

class VulkanSurface 
{
public:
    VulkanSurface(VulkanInstance& instance, void* windowHandle);
    const vk::UniqueSurfaceKHR& Handle() const { return mSurface; }

private:
    vk::UniqueSurfaceKHR mSurface;
};

}