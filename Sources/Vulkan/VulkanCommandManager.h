#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace Lucid {

class VulkanDevice;
class VulkanSwapchain;
class VulkanPipeline;
class VulkanRenderPass;

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