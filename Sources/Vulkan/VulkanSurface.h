#pragma once

#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanEntity.h>

namespace Lucid::Core
{
class IWindow;
}

namespace Lucid::Vulkan
{

class VulkanInstance;

class VulkanSurface : public VulkanEntity<vk::UniqueSurfaceKHR>
{
public:
    VulkanSurface(VulkanInstance& instance, const Core::IWindow& window);
};

}