#pragma once

#include <vulkan/vulkan.hpp>
#include <Vulkan/VulkanEntity.h>

namespace Lucid::Vulkan
{

class VulkanDevice;
class VulkanSwapchain;
class VulkanRenderPass;
class VulkanDescriptorPool;

class VulkanPipeline : public VulkanEntity<vk::UniquePipeline>
{
public:
	VulkanPipeline(
		VulkanDevice& device, 
		const vk::Extent2D& extent, 
		VulkanRenderPass& renderPass, 
		VulkanDescriptorPool& descriptorPool);

	[[nodiscard]] const vk::PipelineLayout& Layout() const;

private:
	vk::UniquePipelineLayout mLayout;
};

}