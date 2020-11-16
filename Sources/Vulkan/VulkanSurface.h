#pragma once

#include <vulkan/vulkan.hpp>

namespace Lucid {

class IWindow;
class VulkanInstance;

class VulkanSurface 
{
public:
    VulkanSurface(VulkanInstance& instance, const IWindow& window);
    const vk::UniqueSurfaceKHR& Handle() const { return mSurface; }

private:
    vk::UniqueSurfaceKHR mSurface;
};

}