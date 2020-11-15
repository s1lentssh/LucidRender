#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include "VulkanDevice.h"

#include <Vulkan/VulkanSwapchain.h>
#include <Vulkan/VulkanPipeline.h>
#include <Vulkan/VulkanRenderPass.h>

namespace lucid {

class VulkanCommandManager
{
public:
	VulkanCommandManager(VulkanDevice& device, VulkanPipeline& pipeline, VulkanRenderPass& renderPass, VulkanSwapchain& swapchain);
	vk::UniqueCommandBuffer& Get(std::size_t index) { return mCommandBuffers.at(index); }

private:

	vk::UniqueCommandPool mCommandPool;
	std::vector<vk::UniqueCommandBuffer> mCommandBuffers;
};

}