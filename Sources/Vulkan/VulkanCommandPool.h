#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanEntity.h>

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanSwapchain;
class VulkanPipeline;
class VulkanRenderPass;
class VulkanVertexBuffer;
class VulkanIndexBuffer;
class VulkanDescriptorPool;

class VulkanCommandPool : public VulkanEntity<vk::UniqueCommandPool>
{
public:
	VulkanCommandPool(
		VulkanDevice& device, 
		VulkanRenderPass& renderPass, 
		VulkanSwapchain& swapchain, 
		VulkanPipeline& pipeline);

	vk::UniqueCommandBuffer& Get(std::size_t index);
	void RecordCommandBuffers(const VulkanVertexBuffer& vertexBuffer, const VulkanIndexBuffer& indexBuffer, const VulkanDescriptorPool& descriptorPool);

private:
	std::vector<vk::UniqueCommandBuffer> mCommandBuffers;

	VulkanSwapchain& mSwapchain;
	VulkanRenderPass& mRenderPass;
	VulkanPipeline& mPipeline;
};

}