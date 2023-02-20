#pragma once

#include <Vulkan/VulkanEntity.h>
#include <vulkan/vulkan.hpp>

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

} // namespace Lucid::Vulkan
