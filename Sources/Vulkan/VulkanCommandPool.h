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
class VulkanMesh;

class VulkanCommandPool : public VulkanEntity<vk::UniqueCommandPool>
{
public:
	VulkanCommandPool(
		VulkanDevice& device, 
		VulkanSwapchain& swapchain, 
		VulkanPipeline& pipeline);

	[[nodiscard]] vk::UniqueCommandBuffer& Get(std::size_t index);

	void RecordCommandBuffers(
		const VulkanRenderPass& renderPass,
		const std::vector<VulkanMesh>& meshes,
		const VulkanDescriptorPool& descriptorPool);

	void ExecuteSingleCommand(const std::function<void(vk::CommandBuffer&)>& function);

private:
	std::vector<vk::UniqueCommandBuffer> mCommandBuffers;

	VulkanSwapchain& mSwapchain;
	VulkanPipeline& mPipeline;
	VulkanDevice& mDevice;
};

}