#pragma once

#include <vulkan/vulkan.hpp>

namespace Lucid
{

class VulkanDevice;
class VulkanSwapchain;
class VulkanRenderPass;

class VulkanPipeline
{
public:
	VulkanPipeline(VulkanDevice& device, const vk::Extent2D& extent, VulkanRenderPass& renderPass);
	vk::UniquePipeline& Handle() { return mPipeline; }

private:
	void Init();

	VulkanDevice& mDevice;
	vk::Extent2D mExtent;
	VulkanRenderPass& mRenderPass;

	vk::UniquePipelineLayout mLayout;
	vk::UniquePipeline mPipeline;
};

}