#pragma once

#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanEntity.h>

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

}
