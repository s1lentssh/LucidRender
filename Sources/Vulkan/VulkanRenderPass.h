#pragma once

#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanSwapchain.h>

namespace lucid
{

class VulkanPipeline;

class VulkanRenderPass
{
public:
	VulkanRenderPass(VulkanDevice& device, const VulkanSwapchain& swapchain);
	vk::UniqueRenderPass& Handle() { return mRenderPass; }

private:
	vk::UniqueRenderPass mRenderPass;
};

}