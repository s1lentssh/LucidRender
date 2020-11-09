#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanSwapchain.h"

namespace lucid
{

class VulkanRenderPass
{
public:
	VulkanRenderPass(VulkanDevice& device, const VulkanSwapchain& swapchain);
	vk::UniqueRenderPass& Handle() { return mRenderPass; }

private:
	vk::UniqueRenderPass mRenderPass;
};

}