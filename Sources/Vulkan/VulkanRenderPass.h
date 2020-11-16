#pragma once

#include <vulkan/vulkan.hpp>

namespace Lucid
{

class VulkanPipeline;
class VulkanSwapchain;
class VulkanDevice;

class VulkanRenderPass
{
public:
	VulkanRenderPass(VulkanDevice& device, const VulkanSwapchain& swapchain);
	vk::UniqueRenderPass& Handle() { return mRenderPass; }

private:
	vk::UniqueRenderPass mRenderPass;
};

}