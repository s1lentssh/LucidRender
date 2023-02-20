#pragma once

#include <Vulkan/VulkanEntity.h>
#include <vulkan/vulkan.hpp>

namespace Lucid::Vulkan
{

class VulkanPipeline;
class VulkanSwapchain;
class VulkanDevice;

class VulkanRenderPass : public VulkanEntity<vk::UniqueRenderPass>
{
public:
    VulkanRenderPass(VulkanDevice& device, vk::Format imageFormat);
};

} // namespace Lucid::Vulkan
