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
	VulkanCommandPool(VulkanDevice& device);

	[[nodiscard]] vk::UniqueCommandBuffer& Get(std::size_t index);

	void RecreateCommandBuffers(VulkanSwapchain& swapchain);

	void RecordCommandBuffers(
		VulkanSwapchain& swapchain,
		const VulkanRenderPass& renderPass,
		std::function<void(vk::CommandBuffer& commandBuffer)> action);

	void ExecuteSingleCommand(const std::function<void(vk::CommandBuffer&)>& function);

private:
	std::vector<vk::UniqueCommandBuffer> mCommandBuffers;

	VulkanDevice& mDevice;
};

}
